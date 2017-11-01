#pragma once

#include <vector>
#include "cg.h"

class ANN
{
public:

    void build(int N, int D, int C);

    ComputationGraph& getGraph();

protected:

    int addLayer(int prev, bool last);

protected:

    int _N;
    int _D;
    int _C;
    ComputationGraph _graph;
    int _front;
    int _back;
    std::vector<int> _weights;
};

