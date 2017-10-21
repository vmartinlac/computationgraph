
#include <iostream>
#include "cg.h"

int main(int num_args, char** args)
{
    ComputationGraph* graph = new ComputationGraph;

    ConstantNode* a = new ConstantNode(graph, 2);
    a->setValue(0, 2.0);
    a->setValue(1, 3.0);

    Multiply* b = new Multiply(graph, 1);
    b->connect(0, a, 0);
    b->connect(1, a, 0);

    Sum* c = new Sum(graph, 1);
    c->connect(0, b, 0);
    c->connect(1, a, 1);

    graph->update();
    std::cout << c->getOutput(0) << std::endl;

    delete graph;

    return 0;
}

