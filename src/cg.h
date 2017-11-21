#pragma once

#include <ostream>
#include <vector>
#include <cassert>

class ComputationGraph
{
public:

   class Evaluation;

   enum
   {
      FUNCTION_CONSTANT,
      FUNCTION_SUM,
      FUNCTION_ARRAY_SUM,
      FUNCTION_ARRAY_SUBSTRACTION,
      FUNCTION_ARRAY_PRODUCT,
      FUNCTION_LOGSOFTMAX,
      FUNCTION_IDENTITY,
      FUNCTION_POSITIVE_PART,
   };

   ComputationGraph();

   int addNode(int function, int dimension);

   void connect(int node_from, int output, int node_to, int input);

   void print(std::ostream& f);

   void check();

   void clear();

   int getNumOutputs(int node);

protected:

   struct Node
   {
      int function;
      int dimension;
      int num_inputs;
      int num_outputs;
      int input_offset;
      int output_offset;
   };

   int _dimension;
   std::vector<Node> _nodes;
   std::vector<int> _inputs;
};

inline int ComputationGraph::getNumOutputs(int node)
{
    assert( 0 <= node && node < _nodes.size() );
    return _nodes[node].num_outputs;
}

class ComputationGraph::Evaluation
{
public:

   Evaluation(ComputationGraph* graph);

   void setValue(int node, int output, double value);

   void update();

   void updateGradient(int node, int output);

   double getValue(int node, int output);

   double& refValue(int node, int output);

   double getGradient(int node, int output);

protected:

   std::vector<double> _values;
   std::vector<double> _gradient;
   ComputationGraph* _graph;
};

