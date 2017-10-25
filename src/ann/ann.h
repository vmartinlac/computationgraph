#pragma once

#include <vector>
#include "cgcore.h"
#include "cgnodes.h"
#include "abstractdataset.h"

class ANN
{
public:

    ANN();
    ~ANN();

    void build_and_train(AbstractDataset* trainingSet, int depth);

    int predict(std::vector<double>& classes);

    int getNumberOfWeights();

protected:

    int _breadth;
    int _depth;
    int _numClasses;
    ComputationGraph* _graph;
    ConstantNode* _inputNode;
    SoftmaxNode* _outputNode;
    std::vector<ConstantNode*> _weights;
};

