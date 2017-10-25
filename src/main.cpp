
#include <iostream>
#include "ann.h"
#include "cg.h"

void exempleI();
void exempleII();
void exempleIII();

int main(int num_args, char** args)
{
    exempleIII();
    return 0;
}

void exempleIII()
{
    ANN ann;
    ann.build(2, 3, 2);
    ann.train();
}

void exempleI()
{
    const double alpha = 1.0;
    const double beta = 2.0;

    ComputationGraph* graph = new ComputationGraph;

    ConstantNode* a = new ConstantNode(graph, 2);
    SumNode* c = new SumNode(graph, 1);
    MultiplyNode* d = new MultiplyNode(graph, 1);
    MultiplyNode* e = new MultiplyNode(graph, 1);
    SumNode* f = new SumNode(graph, 1);

    a->setValue(0, alpha);
    a->setValue(1, beta);
    c->connect(0, a, 0);
    c->connect(1, a, 1);
    d->connect(0, a, 0);
    d->connect(1, a, 1);
    e->connect(0, c, 0);
    e->connect(1, d, 0);
    f->connect(0, e, 0);
    f->connect(1, d, 0);

    graph->update();
    graph->updateGradient(f, 0);

    std::cout << f->getOutput(0) << std::endl;
    std::cout << alpha*beta*(1.0+alpha+beta) << std::endl;

    std::cout << a->getGradient(0) << std::endl;
    std::cout << a->getGradient(1) << std::endl;

    delete graph;
}

void exempleII()
{

    ComputationGraph* graph = new ComputationGraph;

    ConstantNode* a = new ConstantNode(graph, 2);
    MultiplyNode* b = new MultiplyNode(graph, 1);

    a->setValue(0, 3.0);
    a->setValue(1, 2.0);
    b->connect(0, a, 0);
    b->connect(1, a, 1);

    graph->update();
    graph->updateGradient(b, 0);

    std::cout << b->getOutput(0) << std::endl;
    std::cout << a->getGradient(0) << std::endl;
    std::cout << a->getGradient(1) << std::endl;

    delete graph;
}

