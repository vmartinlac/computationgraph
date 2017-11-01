#include "ann.h"

ComputationGraph& ANN::getGraph()
{
    return _graph;
}

int ANN::addLayer(int prev, bool last)
{
    int m = (last) ? _C : _N;

    std::vector<int> table(m);

    for(int n=0; n<m; n++)
    {
        int weight = _graph.addNode(ComputationGraph::FUNCTION_CONSTANT, _N+1);
        int prod = _graph.addNode(ComputationGraph::FUNCTION_ARRAY_PRODUCT, _N);
        table[n] = _graph.addNode(ComputationGraph::FUNCTION_SUM, _N+1);

        for(int l=0; l<_N; l++)
        {
            _graph.connect(prev, l, prod, 2*l+0);
            _graph.connect(weight, 1+l, prod, 2*l+1);
            _graph.connect(prod, l, table[n], 1+l);
        }

        _graph.connect(weight, 0, table[n], 0);

        _weights.push_back(weight);
    }

    int next = (last) ?_graph.addNode(ComputationGraph::FUNCTION_LOGSOFTMAX, m) : _graph.addNode(ComputationGraph::FUNCTION_POSITIVE_PART, m) ;

    for(int l=0; l<m; l++)
    {
        _graph.connect(table[l], 0, next, l);
    }

    return next;
}

void ANN::build(int N, int D, int C)
{
    _N = N;
    _D = D;
    _C = C;
    _graph.clear();

    _front = _graph.addNode(ComputationGraph::FUNCTION_CONSTANT, N);

    int prev = _front;

    for(int d=0; d<D; d++)
    {
        prev = addLayer(prev, false);
    }

    _back = addLayer(prev, true);

    _graph.check();
}

