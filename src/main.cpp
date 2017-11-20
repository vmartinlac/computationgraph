#include <cassert>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cmath>
#include "cg.h"

struct ANN
{
    ComputationGraph graph;
    int input_node;
    int input_dim;
    int weight_node;
    int weight_dim;
    int output_node;
    int output_dim;
};

class ANNBuilder
{
public:
    ANNBuilder();
    void begin(int input_size);
    void addLayer(int size);
    ANN* end(int function);
protected:
    int mPrevNode;
    int mPrevDim;
    ANN* mANN;
};

ANNBuilder::ANNBuilder() :
    mANN(nullptr),
    mPrevNode(-1),
    mPrevDim(0)
{
    ;
}

void ANNBuilder::begin(int input_size)
{
    assert(mANN == nullptr);

    mANN = new ANN;

    mPrevNode = mANN->input_node = mANN->graph.addNode(ComputationGraph::FUNCTION_CONSTANT, input_size);
    mPrevDim = mANN->input_dim = input_size;
}

void ANNBuilder::addLayer(int size)
{
    ;
}

ANN* ANNBuilder::end(int function)
{
    ANN* ret = mANN;
    mANN = nullptr;

    if(function == ComputationGraph::FUNCTION_SUM)
    {
    }
    else if(function == ComputationGraph::FUNCTION_LOGSOFTMAX)
    {
    }
    else
    {
        throw std::runtime_error("Incorrect function.");
    }

    ret->graph.check();

    return ret;
}


int main(int num_args, char** args)
{
    ANNBuilder builder;
    builder.begin(1);
    builder.addLayer(10);
    builder.addLayer(10);
    ANN* ann = builder.end(ComputationGraph::FUNCTION_SUM);

    ComputationGraph::Evaluation eval(&ann->graph);

    eval.setValue(ann->input_node, 0, 0.0);

    eval.update();
    eval.updateGradient(ann->output_node, 0);

    ann->graph.print(std::cout);
}

