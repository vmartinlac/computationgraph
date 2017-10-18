#include "cgcore.h"

ComputationGraph::ComputationGraph() :
    _frozen(false),
    _numOutputs(0)
{ }

void ComputationGraph::registerNode(ComputationGraph::Node* node, int& id, int& outputsoffset)
{
    assert(_frozen == false);

    id = int(_nodes.size());
    outputsoffset = _numOutputs;

    _nodes.push_back( RegisteredNode(node) );
    _numOutputs += node->getNumOutputs();
}

void ComputationGraph::freeze()
{
    if(_frozen == false)
    {
        _frozen = true;
        // todo : check that all inputs are connected.
        // and that adjacet nodes belong to the same graph.
    }
}

ComputationGraph::~ComputationGraph()
{
    for(RegisteredNode& n : _nodes)
    {
        delete n.node;
    }
}
