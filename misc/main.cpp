#include <iostream>
#include <cassert>
#include <vector>

class ComputationGraph
{
public:

   class Evaluation;

   enum
   {
      FUNCTION_CONSTANT,
      FUNCTION_ARRAY_SUM,
      FUNCTION_ARRAY_PRODUCT,
      FUNCTION_SOFTMAX,
      FUNCTION_IDENTITY,
      FUNCTION_POSITIVE_PART,
      FUNCTION_SCALAR_PRODUCT
   };

   ComputationGraph();

   int addNode(int function, int dimension);

   void connect(int node_from, int output, int node_to, int input);

   void check();

protected:

   struct Node
   {
      int function;
      int dimension;
      int num_inputs;
      int num_outputs;
      int input_offset;
      int output_offset;
   };

   int _dimension;
   std::vector<Node> _nodes;
   std::vector<int> _inputs;
};

class ComputationGraph::Evaluation
{
public:

   Evaluation(ComputationGraph* graph);

   void setValue(int node, int output, double value);

   void update();

   void updateGradient(int node, int output);

   double getValue(int node, int output);

   double getGradient(int node, int output);

protected:

   std::vector<double> _values;
   std::vector<double> _gradient;
   ComputationGraph* _graph;
};

//////////////

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
   case FUNCTION_ARRAY_SUM:
   case FUNCTION_ARRAY_PRODUCT:
      n.num_inputs = 2*dimension;
      n.num_outputs = dimension;
      break;
   case FUNCTION_SOFTMAX:
   case FUNCTION_IDENTITY:
   case FUNCTION_POSITIVE_PART:
      n.num_inputs = dimension;
      n.num_outputs = dimension;
      break;
   case FUNCTION_SCALAR_PRODUCT:
      n.num_inputs = 2*dimension;
      n.num_outputs = 1;
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
   for(Node& n : _graph->_nodes)
   {
      switch(n.function)
      {

      case FUNCTION_CONSTANT:
         break;

      case FUNCTION_ARRAY_PRODUCT:
         for(int i=0; i<n.dimension; i++)
         {
            _values[ n.output_offset + i ] =
               _values[ _graph->_inputs[n.input_offset + 2*i] ] * _values[ _graph->_inputs[n.input_offset + 2*i + 1 ]];
         }
         break;

      case FUNCTION_ARRAY_SUM:
         for(int i=0; i<n.dimension; i++)
         {
            _values[ n.output_offset + i ] =
               _values[ _graph->_inputs[n.input_offset + 2*i] ] + _values[ _graph->_inputs[n.input_offset + 2*i + 1 ]];
         }
         break;

      case FUNCTION_SOFTMAX:
         break;

      case FUNCTION_IDENTITY:
         for(int i=0; i<n.dimension; i++)
         {
            _values[ n.output_offset + i ] = _values[ _graph->_inputs[n.input_offset + i] ];
         }
         break;

      case FUNCTION_POSITIVE_PART:
         for(int i=0; i<n.dimension; i++)
         {
            const double val = _values[ _graph->_inputs[n.input_offset + i] ];
            if(val >= 0.0)
            {
               _values[ n.output_offset + i ] = val;
            }
            else
            {
               _values[ n.output_offset + i ] = 0.0;
            }
         }
         break;

      case FUNCTION_SCALAR_PRODUCT:
         break;

      default:
         throw std::runtime_error("Unkown function.");
      }
   }
}

void ComputationGraph::Evaluation::updateGradient(int node, int output)
{
   _gradient.assign( _graph->_dimension, 0.0 );
   _gradient[ _graph->_nodes[node].output_offset + output ] = 1.0;

   for(std::vector<Node>::reverse_iterator it=_graph->_nodes.rbegin(); it!=_graph->_nodes.rend(); it++)
   {
      ;
   }
}

int main(int num_args, char** args)
{
   ComputationGraph graph;
   int source = graph.addNode(ComputationGraph::FUNCTION_CONSTANT, 2);
   int sum = graph.addNode(ComputationGraph::FUNCTION_ARRAY_SUM, 1);
   int sum2 = graph.addNode(ComputationGraph::FUNCTION_ARRAY_SUM, 1);
   graph.connect(source, 0, sum, 0);
   graph.connect(source, 1, sum, 1);
   graph.connect(sum, 0, sum2, 0);
   graph.connect(source, 0, sum2, 1);
   graph.check();

   ComputationGraph::Evaluation eval(&graph);
   eval.setValue(source, 0, 1.0);
   eval.setValue(source, 1, 3.0);
   eval.update();
   std::cout << eval.getValue(sum2, 0) << std::endl;

   return 0;
}
