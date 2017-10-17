#include "cgcore.h"

ComputationGraph::ComputationGraph() :
    _frozen(false),
    _numOutputs(0)
{ }

int ComputationGraph::registerNode(std::shared_ptr<ComputationGraph::Node> node)
{
    assert(_frozen == false);

    _nodes.push_back( RegisteredNode(node) );
    const int ret = _numOutputs;
    _numOutputs += node->getNumOutputs();
    return ret;
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
