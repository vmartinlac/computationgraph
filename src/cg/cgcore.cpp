#include <algorithm>
#include <iostream>
#include "cgcore.h"

ComputationGraph::ComputationGraph() :
  _frozen(false),
  _nextPass(0)
{ }

ComputationGraph::~ComputationGraph()
{
  for(Node* n : _nodes)
  {
      delete n;
  }
}

void ComputationGraph::freeze()
{
  if(_frozen == false)
  {
      _frozen = true;
      for(Node* n : _nodes)
      {
          assert(n->isReady());
          assert(n->owner() == this);
      }
  }
}


int ComputationGraph::getNumberOfNodes()
{
    return _nodes.size();
}

int ComputationGraph::getNumberOfOutputs()
{
    int ret = 0;
    for(Node* n : _nodes)
    {
        ret += n->getNumOutputs();
    }
    return ret;
}

void ComputationGraph::update()
{
  for(Node* n : _nodes)
  {
      n->update(_nextPass);
  }
  _nextPass++;
}

void ComputationGraph::updateGradient(Node* node, int output)
{
  assert(this == node->owner());

  for(Node* n : _nodes)
  {
      n->prepareGradientUpdate();
  }

  node->setToDifferentiate(output);

  for(Node* n : _nodes)
  {
      n->updateGradient(_nextPass);
  }

  _nextPass++;
}

void ComputationGraph::Node::connect(int input_id, ComputationGraph::Node* source, int output_id)
{
  assert( _graph->_frozen == false );
  assert( 0 <= input_id && input_id < _numInputs );
  assert( 0 <= output_id && output_id < source->getNumOutputs() );
  assert( source->_graph == _graph );
  _inputs[input_id].source = source;
  _inputs[input_id].output_id = output_id;
  source->_clients.insert(this);
}

bool ComputationGraph::Node::isReady()
{
   return std::all_of( _inputs.begin(), _inputs.end(), [] (const Input& c) {
      return c.isConnected();
   });
}

void ComputationGraph::Node::update(int pass)
{
  if(pass != _lastPass)
  {
      _lastPass = pass;
      for(Input& conn : _inputs)
      {
          conn.source->update(pass);
      }
      update();
  }
}

void ComputationGraph::Node::updateGradient(int pass)
{
  if(pass != _lastPass)
  {
      _lastPass = pass;
      for(Node* n : _clients)
      {
          n->updateGradient(pass);
      }
      updateGradient();
  }
}

void ComputationGraph::Node::setToDifferentiate(int output)
{
  assert(0 <= output && output < _numOutputs);
  assert( _clients.empty() );
  for(int i=0; i<_numOutputs; i++)
  {
      _outputs[i].gradient = (i == output) ? 1.0 : 0.0;
  }
}

void ComputationGraph::Node::prepareGradientUpdate()
{
  for(Output& o : _outputs)
  {
      o.gradient = 0.0;
  }
}

ComputationGraph::Node::Node(ComputationGraph* graph, int num_inputs, int num_outputs) :
  _graph(graph),
  _numInputs(num_inputs),
  _numOutputs(num_outputs),
  _inputs(num_inputs),
  _outputs(num_outputs),
  _lastPass(-1)
{
    graph->_nodes.push_back(this);
}

