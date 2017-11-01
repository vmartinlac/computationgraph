#pragma once

#include <vector>
#include "cg.h"

class AbstractDataset
{
public:

    virtual int getNumberOfExamples() = 0;
    virtual int getInputDimension() = 0;
    virtual int getNumberOfClasses() = 0;
    virtual void getExample(int id, ComputationGraph::Evaluation* eval, int node, int& cl) = 0;
};
