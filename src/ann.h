#pragma once

#include <vector>
#include "cgcore.h"
#include "cgnodes.h"

class ANN
{
public:
    ANN();
    ~ANN();

    void build(int breadth, int depth, int num_classes);
    void train();
    bool predict(double rgb[3]);

protected:

    int _breadth;
    int _depth;
    int _numClasses;
    ComputationGraph* _graph;
    ConstantNode* _inputNode;
    std::vector<ConstantNode*> _weights;
};
