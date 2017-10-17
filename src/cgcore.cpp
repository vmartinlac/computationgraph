#include "cgcore.h"

ComputationGraph::ComputationGraph()
    _frozen(false),
    _numOutputs(0)
{ }

void ComputationGraph::registerNode(std::shared_ptr<ComputationGraph::Node> node)
{
    assert(_frozen == false);

    _nodes.emplace(node);
    node->setGraph(this);
    node->setOutputsOffset(_numOutputs);
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
