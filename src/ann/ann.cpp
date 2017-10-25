#include "ann.h"
#include <random>
#include <iostream>

void ANN::build_and_train(AbstractDataset* set, int depth)
{
    assert( _graph == nullptr );
    assert( _inputNode == nullptr );
    assert( _weights.empty() );

    _breadth = set->getInputDimension();
    _depth = depth;
    _numClasses = set->getNumberOfClasses();
    _graph = new ComputationGraph;

    ConstantNode* one_node = new ConstantNode(_graph, 1);
    one_node->setValue(0, 1.0);

    _inputNode = new ConstantNode(_graph, _breadth);

    ComputationGraph::Node* prev = _inputNode;
    for(int i=0; i<_depth; i++)
    {
        ComputationGraph::Node* next = new PositivePartNode(_graph, _breadth);

        for(int j=0; j<_breadth; j++)
        {
            ComputationGraph::Node* n = new ScalarProductNode(_graph, _breadth+1);
            ConstantNode* weights = new ConstantNode(_graph, _breadth+1);

            for(int k=0; k<_breadth; k++)
            {
                n->connect(k, prev, k);
                n->connect(k+_breadth+1, weights, k);
            }
            n->connect(_breadth, one_node, 0);
            n->connect(_breadth*2+1, weights, _breadth);
            _weights.push_back(weights);
            next->connect(j, n, 0);
        }

        prev = next;
    }

    SoftmaxNode* final_node = new SoftmaxNode(_graph, _numClasses);
    _outputNode = final_node;

    for(int i=0; i<_numClasses; i++)
    {
        ComputationGraph::Node* n = new ScalarProductNode(_graph, _breadth+1);
        ConstantNode* weights = new ConstantNode(_graph, _breadth+1);

        for(int k=0; k<_breadth; k++)
        {
            n->connect(k, prev, k);
            n->connect(k+_breadth+1, weights, k);
        }
        n->connect(_breadth, one_node, 0);
        n->connect(_breadth*2+1, weights, _breadth);
        _weights.push_back(weights);
        final_node->connect(i, n, 0);
    }

    _graph->freeze();

    std::cout << "ANN built." << std::endl;
    std::cout << _graph->getNumberOfNodes() << " nodes." << std::endl;
    std::cout << _graph->getNumberOfOutputs() << " variables." << std::endl;
    std::cout << getNumberOfWeights() << " weights." << std::endl;

    //////
    // random weights
    //
    std::default_random_engine engine;
    std::normal_distribution<double> distrib;
    engine.seed(12);

    for(ConstantNode* n : _weights)
    {
        for(int i=0; i<n->getNumOutputs(); i++)
        {
            n->setValue(i, distrib(engine));
        }
    }

    //////

    std::vector<double> input;
    int cl;
    set->getExample(10, input, cl);

    assert(input.size() == _breadth);
    for(int i=0; i<_breadth; i++)
    {
        _inputNode->setValue(i, input[i]);
    }
    _graph->update();
    for(int i=0; i<_numClasses; i++)
    {
        std::cout << _outputNode->getOutput(i) << std::endl;
    }
}

ANN::ANN() :
    _graph(nullptr),
    _inputNode(nullptr)
{
    ;
}

ANN::~ANN()
{
    if(_graph != nullptr)
    {
        delete _graph;
    }
}

int ANN::getNumberOfWeights()
{
    int ret = 0;

    for(ConstantNode* n : _weights)
    {
        ret += n->getNumOutputs();
    }

    return ret;
}
