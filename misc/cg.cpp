#include "cg.h"
#include <cassert>
#include <cmath>
#include <stdexcept>

ComputationGraph::ComputationGraph() : _dimension(0) { }

int ComputationGraph::addNode(int function, int dimension)
{
   Node n;

   n.function = function;
   n.dimension = dimension;

   switch(function)
   {
   case FUNCTION_CONSTANT:
      n.num_inputs = 0;
      n.num_outputs = dimension;
      break;
   case FUNCTION_SUM:
      n.num_inputs = dimension;
      n.num_outputs = 1;
      break;
   case FUNCTION_ARRAY_SUM:
   case FUNCTION_ARRAY_PRODUCT:
      n.num_inputs = 2*dimension;
      n.num_outputs = dimension;
      break;
   case FUNCTION_LOGSOFTMAX:
      n.num_inputs = dimension;
      n.num_outputs = dimension;
      break;
   case FUNCTION_POSITIVE_PART:
      n.num_inputs = dimension;
      n.num_outputs = dimension;
      break;
   default:
      throw std::runtime_error("Unexpected value for 'function'.");
   }

   n.input_offset = _inputs.size();
   n.output_offset = _dimension;

   _dimension += n.num_outputs;
   for(int i=0; i<n.num_inputs; i++)
   {
      _inputs.push_back(-1);
   }

   _nodes.push_back(n);

   return _nodes.size()-1;
}

void ComputationGraph::connect(int node_from, int output, int node_to, int input)
{
   Node& nfrom = _nodes[node_from];
   Node& nto = _nodes[node_to];

   assert( 0 <= output && output < nfrom.num_outputs );
   assert( 0 <= input && input < nto.num_inputs );

   _inputs[ nto.input_offset + input ] = nfrom.output_offset + output;
}

void ComputationGraph::check()
{
   for(int i : _inputs)
   {
      assert(i >= 0);
   }

   std::vector<bool> set(_dimension, false);
   for(Node& n : _nodes)
   {
      for(int i=0; i<n.num_inputs; i++)
      {
         assert( set[ _inputs[n.input_offset+i] ] );
      }
      for(int i=0; i<n.num_outputs; i++)
      {
         set[ n.output_offset+i ] = true;
      }
   }
}

ComputationGraph::Evaluation::Evaluation(ComputationGraph* graph) : _graph(graph), _values(graph->_dimension), _gradient(graph->_dimension) { }

void ComputationGraph::Evaluation::setValue(int node, int output, double value)
{
   ComputationGraph::Node& n = _graph->_nodes[node];
   assert( 0 <= output && output < n.num_outputs );
   _values[ n.output_offset + output ] = value;
}

double ComputationGraph::Evaluation::getValue(int node, int output)
{
   ComputationGraph::Node& n = _graph->_nodes[node];
   assert( 0 <= output && output < n.num_outputs );
   return _values[ n.output_offset + output ];
}

double ComputationGraph::Evaluation::getGradient(int node, int output)
{
   ComputationGraph::Node& n = _graph->_nodes[node];
   assert( 0 <= output && output < n.num_outputs );
   return _gradient[ n.output_offset + output ];
}

void ComputationGraph::Evaluation::update()
{
   for(std::vector<Node>::iterator n=_graph->_nodes.begin(); n!=_graph->_nodes.end(); n++)
   {
      switch(n->function)
      {
      case FUNCTION_CONSTANT:
         break;

      case FUNCTION_SUM:
         {
            double val = 0.0;
            for(int i=0; i<n->dimension; i++)
            {
                val += _values[ _graph->_inputs[ n->input_offset + i ] ];
            }
            _values[ n->output_offset ] = val;
         }
         break;

      case FUNCTION_ARRAY_PRODUCT:
         for(int i=0; i<n->dimension; i++)
         {
            _values[ n->output_offset + i ] =
               _values[ _graph->_inputs[n->input_offset + 2*i] ] * _values[ _graph->_inputs[n->input_offset + 2*i + 1 ]];
         }
         break;

      case FUNCTION_ARRAY_SUM:
         for(int i=0; i<n->dimension; i++)
         {
            _values[ n->output_offset + i ] =
               _values[ _graph->_inputs[n->input_offset + 2*i] ] + _values[ _graph->_inputs[n->input_offset + 2*i + 1 ]];
         }
         break;

      case FUNCTION_POSITIVE_PART:
         for(int i=0; i<n->dimension; i++)
         {
            const double val = _values[ _graph->_inputs[n->input_offset + i] ];
            if(val >= 0.0)
            {
               _values[ n->output_offset + i ] = val;
            }
            else
            {
               _values[ n->output_offset + i ] = 0.0;
            }
         }
         break;

      case FUNCTION_LOGSOFTMAX:
         {
            // find max input value.
            double max = _values[ _graph->_inputs[ n->input_offset ] ];
            for(int i=1; i<n->dimension; i++)
            {
                max = std::max(max, _values[ _graph->_inputs[ n->input_offset + i ] ] );
            }

            // compute the sum.
            double sum = 0.0;
            for(int i=0; i<n->dimension; i++)
            {
                sum += std::exp( _values[ _graph->_inputs[ n->input_offset + i ] ] - max );
            }

            // set the outputs.
            for(int i=0; i<n->dimension; i++)
            {
                //_values[ n->output_offset + 2*i ] = std::exp( _values[ _graph->_inputs[ n->input_offset + i ] ] - max ) / sum;
                //_values[ n->output_offset + 2*i + 1 ] = _values[ _graph->_inputs[ n->input_offset + i ] ] - max - std::log(sum);

                _values[ n->output_offset + i ] = _values[ _graph->_inputs[ n->input_offset + i ] ] - max - std::log(sum);
            }
         }
         break;

      default:
         throw std::runtime_error("Unkown function.");
      }
   }
}

void ComputationGraph::Evaluation::updateGradient(int node, int output)
{
    std::vector<bool> immutable(_graph->_dimension, false);

   _gradient.assign( _graph->_dimension, 0.0 );
   _gradient[ _graph->_nodes[node].output_offset + output ] = 1.0;

   for(std::vector<Node>::reverse_iterator n=_graph->_nodes.rbegin(); n!=_graph->_nodes.rend(); n++)
   {
      for(int i=0; i<n->num_outputs; i++)
      {
        immutable[ n->output_offset + i] = true;
      }

      for(int i=0; i<n->num_inputs; i++)
      {
        assert( immutable[ _graph->_inputs[n->input_offset + i]] == false );
      }

      switch(n->function)
      {

      case FUNCTION_CONSTANT:
         break;

      case FUNCTION_SUM:
         for(int i=0; i<n->dimension; i++)
         {
            _gradient[ _graph->_inputs[n->input_offset + i] ] += _gradient[ n->output_offset ];
         }
         break;

      case FUNCTION_ARRAY_PRODUCT:
         for(int i=0; i<n->dimension; i++)
         {
            _gradient[ _graph->_inputs[n->input_offset + 2*i] ] +=
                _values[ _graph->_inputs[n->input_offset + 2*i + 1 ]] * _gradient[ n->output_offset + i ];
            _gradient[ _graph->_inputs[n->input_offset + 2*i + 1] ] +=
                _values[ _graph->_inputs[n->input_offset + 2*i ]] * _gradient[ n->output_offset + i ];
         }
         break;

      case FUNCTION_ARRAY_SUM:
         for(int i=0; i<n->dimension; i++)
         {
           _gradient[ _graph->_inputs[n->input_offset + 2*i + 0] ] += _gradient[ n->output_offset + i ];
           _gradient[ _graph->_inputs[n->input_offset + 2*i + 1] ] += _gradient[ n->output_offset + i ];
         }
         break;

      case FUNCTION_LOGSOFTMAX:
         {
            // find max input value.
            double max = _values[ _graph->_inputs[ n->input_offset ] ];
            for(int i=1; i<n->dimension; i++)
            {
                max = std::max(max, _values[ _graph->_inputs[ n->input_offset + i ] ] );
            }

            // compute the sum.
            double sum = 0.0;
            for(int i=0; i<n->dimension; i++)
            {
                sum += std::exp( _values[ _graph->_inputs[ n->input_offset + i ] ] - max );
            }

            // set the outputs.
            for(int i=0; i<n->dimension; i++)
            {
                double& grad = _gradient[ _graph->_inputs[ n->input_offset + i ] ];

                grad += _gradient[ n->output_offset + i ];

                for(int j=0; j<n->dimension; j++)
                {
                    grad -= std::exp( _values[ _graph->_inputs[ n->input_offset + i ] ] - max) * _gradient[ n->output_offset + j ] / sum;
                }
            }
         }
         break;

      case FUNCTION_POSITIVE_PART:
         for(int i=0; i<n->dimension; i++)
         {
            const double val = _values[ _graph->_inputs[n->input_offset + i] ];
            if(val >= 0.0)
            {
               _gradient[ _graph->_inputs[n->input_offset + i] ] += _gradient[ n->output_offset + i ];
            }
         }
         break;

      default:
         throw std::runtime_error("Unkown function.");
      }
   }
}
