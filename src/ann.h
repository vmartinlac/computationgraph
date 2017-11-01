#pragma once

#include <vector>
#include "cg.h"
#include "abstractdataset.h"

class ANN
{
public:

    void build(int N, int D, int C);

    void train(AbstractDataset* dataset);

    ComputationGraph& getGraph();

protected:

    int addLayer(int prev, bool last);
    void setRandomWeights(ComputationGraph::Evaluation* eval);

protected:

    int _N;
    int _D;
    int _C;
    ComputationGraph _graph;
    int _front;
    int _back;
    std::vector<int> _weights;
};

