
#include <iostream>
#include "cg.h"

void exempleI();
void exempleII();

int main(int num_args, char** args)
{
    exempleII();

    return 0;
}

void exempleI()
{
    const double alpha = -3.0;
    const double beta = 2.0;

    ComputationGraph* graph = new ComputationGraph;

    ConstantNode* a = new ConstantNode(graph, 1);
    ConstantNode* b = new ConstantNode(graph, 1);
    Sum* c = new Sum(graph, 1);
    Multiply* d = new Multiply(graph, 1);
    Multiply* e = new Multiply(graph, 1);
    Sum* f = new Sum(graph, 1);

    a->setValue(0, alpha);
    b->setValue(0, beta);
    c->connect(0, a, 0);
    c->connect(1, b, 0);
    d->connect(0, a, 0);
    d->connect(1, b, 0);
    e->connect(0, c, 0);
    e->connect(1, d, 0);
    f->connect(0, e, 0);
    f->connect(1, d, 0);

    graph->update();
    std::cout << f->getOutput(0) << std::endl;
    std::cout << alpha*beta*(1.0+alpha+beta) << std::endl;

    delete graph;
}

void exempleII()
{

    ComputationGraph* graph = new ComputationGraph;

    ConstantNode* a = new ConstantNode(graph, 1);
    Multiply* b = new Multiply(graph, 1);

    a->setValue(0, 3.0);
    b->connect(0, a, 0);
    b->connect(1, a, 0);

    graph->update();
    graph->updateGradient(b, 0);

    std::cout << b->getOutput(0) << std::endl;
    std::cout << a->getGradient(0) << std::endl;

    delete graph;
}

