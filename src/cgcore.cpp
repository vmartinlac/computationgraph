#include "cgcore.h"

ComputationGraph::ComputationGraph() :
    _frozen(false),
    _nextPass(0)
{ }

void ComputationGraph::registerNode(ComputationGraph::Node* node)
{
    assert(_frozen == false);
    _nodes.push_back( RegisteredNode(node) );
}

void ComputationGraph::freeze()
{
    if(_frozen == false)
    {
        _frozen = true;
        for(RegisteredNode& n : _nodes)
        {
            assert(n.node->isReady());
            assert(n.node->getGraph() == this);
        }
    }
}

ComputationGraph::~ComputationGraph()
{
    for(RegisteredNode& n : _nodes)
    {
        delete n.node;
    }
}

void ComputationGraph::update()
{
    for(RegisteredNode& n : _nodes)
    {
        n.node->update(_nextPass);
    }
    _nextPass++;
}

void ComputationGraph::computeGradient(Node* n, int output_id)
{
   for(RegisteredNode& rn : _nodes)
   {
      rn.node->setZeroGradient();
   }
   n->computeGradient(output_id, _nextPass);
   _nextPass++;
}
