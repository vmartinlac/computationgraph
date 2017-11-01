#include <iostream>
#include <vector>
#include <cmath>
#include "cg.h"
#include "ann.h"

int exempleI();
int exempleII();

int main(int num_args, char** args)
{
    return exempleII();
}

int exempleII()
{
    ANN ann;
    ann.build(5, 1, 3);
    ann.getGraph().print(std::cout);
}

int exempleI()
{
   ComputationGraph graph;
   int source = graph.addNode(ComputationGraph::FUNCTION_CONSTANT, 3);
   int end = graph.addNode(ComputationGraph::FUNCTION_LOGSOFTMAX, 3);

   graph.connect(source, 0, end, 0);
   graph.connect(source, 1, end, 1);
   graph.connect(source, 2, end, 2);

   graph.check();

   ComputationGraph::Evaluation eval(&graph);

   eval.setValue(source, 0, 2.0);
   eval.setValue(source, 1, 2.0);
   eval.setValue(source, 2, 2.0);

   eval.update();
   eval.updateGradient(end, 2);

   /*
   std::cout << std::exp( eval.getValue(end, 0) ) << std::endl;
   std::cout << std::exp( eval.getValue(end, 1) ) << std::endl;
   std::cout << std::exp( eval.getValue(end, 2) ) << std::endl;
   std::cout << std::endl;
   std::cout << eval.getGradient(source, 0) << std::endl;
   std::cout << eval.getGradient(source, 1) << std::endl;
   std::cout << eval.getGradient(source, 2) << std::endl;
   */

   graph.print(std::cout);

   return 0;
}

