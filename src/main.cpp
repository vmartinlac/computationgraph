#include <cassert>
#include <stdexcept>
#include <random>
#include <iostream>
#include <vector>
#include <cmath>
#include "cg.h"

struct ANN
{
    ComputationGraph graph;
    int input_node;
    std::vector<int> weight_nodes;
    int output_node;
    int reference_node;
    int error_node;
};

class ANNBuilder
{
public:
    ANNBuilder(ANN* ann);
    void addInputLayer(int input_size);
    void addHiddenLayer(int size);
    void addOutputLayer();
    void addErrorLayer();
protected:
    int mPrevNode;
    int mPrevDim;
    ANN* mANN;
};

ANNBuilder::ANNBuilder(ANN* ann) :
    mANN(ann),
    mPrevNode(-1),
    mPrevDim(0)
{
    ;
}

void ANNBuilder::addInputLayer(int input_size)
{
    mPrevNode = mANN->input_node = mANN->graph.addNode(ComputationGraph::FUNCTION_CONSTANT, input_size);
    mPrevDim = input_size;
}

void ANNBuilder::addHiddenLayer(int size)
{
    std::vector<int> sum_nodes;
    for(int i=0; i<size; i++)
    {
        const int weight_node = mANN->graph.addNode(ComputationGraph::FUNCTION_CONSTANT, mPrevDim+1);
        const int multiply_node = mANN->graph.addNode(ComputationGraph::FUNCTION_ARRAY_PRODUCT, mPrevDim);
        const int sum_node = mANN->graph.addNode(ComputationGraph::FUNCTION_SUM, mPrevDim+1);

        sum_nodes.push_back(sum_node);
        mANN->weight_nodes.push_back(weight_node);

        for(int j=0; j<mPrevDim; j++)
        {
            mANN->graph.connect(mPrevNode, j, multiply_node, 2*j);
            mANN->graph.connect(weight_node, 1+j, multiply_node, 2*j+1);
            mANN->graph.connect(multiply_node, j, sum_node, 1+j);
        }
        mANN->graph.connect(weight_node, 0, sum_node, 0);
    }

    mPrevNode = mANN->graph.addNode(ComputationGraph::FUNCTION_POSITIVE_PART, size);
    mPrevDim = size;

    for(int j=0; j<size; j++)
    {
        mANN->graph.connect(sum_nodes[j], 0, mPrevNode, j);
    }
}

void ANNBuilder::addOutputLayer()
{
    const int weight_node = mANN->graph.addNode(ComputationGraph::FUNCTION_CONSTANT, mPrevDim+1);
    const int multiply_node = mANN->graph.addNode(ComputationGraph::FUNCTION_ARRAY_PRODUCT, mPrevDim);
    const int sum_node = mANN->graph.addNode(ComputationGraph::FUNCTION_SUM, mPrevDim+1);

    mANN->output_node = sum_node;
    mANN->weight_nodes.push_back(weight_node);

    for(int j=0; j<mPrevDim; j++)
    {
        mANN->graph.connect(mPrevNode, j, multiply_node, 2*j);
        mANN->graph.connect(weight_node, 1+j, multiply_node, 2*j+1);
        mANN->graph.connect(multiply_node, j, sum_node, 1+j);
    }
    mANN->graph.connect(weight_node, 0, sum_node, 0);
}

void ANNBuilder::addErrorLayer()
{
    mANN->reference_node = mANN->graph.addNode(ComputationGraph::FUNCTION_CONSTANT, 1);
    const int sub_node = mANN->graph.addNode(ComputationGraph::FUNCTION_ARRAY_SUBSTRACTION, 1);
    mANN->error_node = mANN->graph.addNode(ComputationGraph::FUNCTION_ARRAY_PRODUCT, 1);

    mANN->graph.connect( mANN->reference_node, 0, sub_node, 0 );
    mANN->graph.connect( mANN->output_node, 0, sub_node, 1 );
    mANN->graph.connect( sub_node, 0, mANN->error_node, 0 );
    mANN->graph.connect( sub_node, 0, mANN->error_node, 1 );
}

void initialize_weights(ANN& ann, ComputationGraph::Evaluation& eval)
{
    std::default_random_engine engine;
    std::normal_distribution<double> distribution;

    const int num_weight_nodes = ann.weight_nodes.size();

    for(int i=0; i<num_weight_nodes; i++)
    {
        const int weight_node = ann.weight_nodes[i];
        const int num_outputs = ann.graph.getNumOutputs(weight_node);

        eval.setValue(weight_node, 0, 0.0);

        for(int j=1; j<num_outputs; j++)
        {
            eval.refValue(weight_node, j) = distribution(engine) / double(num_outputs-1);
        }
    }
}

void gradient_descent_step(ANN& ann, ComputationGraph::Evaluation& eval, double step)
{
    const int num_weight_nodes = ann.weight_nodes.size();

    std::vector< std::vector<double> > gradients(num_weight_nodes);
    for(int i=0; i<num_weight_nodes; i++)
    {
        gradients[i].assign(ann.graph.getNumOutputs(ann.weight_nodes[i]), 0.0);
    }

    double error = 0.0;

    for(double pt=0.0; pt<=10.0; pt+=0.01)
    {
        eval.setValue(ann.input_node, 0, pt);
        //eval.setValue(ann.reference_node, 0, pt);
        eval.setValue(ann.reference_node, 0, pt*(pt-10.0)/25.0 );
        eval.update();
        eval.updateGradient(ann.error_node, 0);

        for(int i=0; i<num_weight_nodes; i++)
        {
            const int weight_node = ann.weight_nodes[i];
            const int num_outputs = ann.graph.getNumOutputs(weight_node);

            for(int j=0; j<num_outputs; j++)
            {
                gradients[i][j] += eval.getGradient(weight_node, j);
            }
        }

        error += eval.getValue(ann.error_node, 0);
    }

    std::cout << error << std::endl;

    for(int i=0; i<num_weight_nodes; i++)
    {
        const int weight_node = ann.weight_nodes[i];
        const int num_outputs = ann.graph.getNumOutputs(weight_node);

        for(int j=0; j<num_outputs; j++)
        {
            eval.refValue(weight_node, j) -= step*gradients[i][j];
        }
    }
}

int main(int num_args, char** args)
{
    ANN ann;

    ANNBuilder builder(&ann);
    builder.addInputLayer(1);
    //builder.addHiddenLayer(20);
    builder.addHiddenLayer(20);
    builder.addHiddenLayer(20);
    builder.addOutputLayer();
    builder.addErrorLayer();

    ann.graph.check();

    ComputationGraph::Evaluation eval(&ann.graph);

    initialize_weights(ann, eval);

    for(int s=0; s<10000; s++)
    {
        std::cout << "iter=" << s << std::endl;
        gradient_descent_step(ann, eval, 0.0001);
    }

    for(double t=0.0; t<=10.0; t+=0.1)
    {
        eval.setValue(ann.input_node, 0, t);
        eval.update();
        std::cout << t << '\t' << eval.getValue(ann.output_node, 0) << std::endl;
    }

    //ann.graph.print(std::cout);
}

