#include "ann.h"
#include <random>
#include <iostream>

void ANN::build(int breadth, int depth, int num_classes)
{
    assert(_graph == nullptr);
    assert(_inputNode == nullptr);
    assert(_weights.empty());

    _breadth = breadth;
    _depth = depth;
    _numClasses = num_classes;
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
}

void ANN::train()
{
    std::default_random_engine engine;
    std::normal_distribution<double> distrib;
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
