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

  node->updateGradient(output, _nextPass);

  for(Node* n : _nodes)
  {
      n->updateGradient(_nextPass);
  }

  _nextPass++;
}
