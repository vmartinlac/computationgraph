#include "cgnodes.h"

// Constant node.

ConstantNode::ConstantNode(ComputationGraph* graph, int dim) : Node(graph, 0, dim) { }

void ConstantNode::setValue(int id, double value)
{
   setOutput(id, value);
}

const char* ConstantNode::name()
{
   return "Constant";
}

void ConstantNode::update()
{
   ;
}

void ConstantNode::updateGradient()
{
   ;
}

// Fraction node.

FractionNode::FractionNode(ComputationGraph* graph, int dim) : Node(graph, dim, dim) { }

const char* FractionNode::name() { return "Fraction"; }

void FractionNode::update()
{
  assert( getNumInputs() == getNumOutputs() );
  for(int i=0; i<getNumInputs(); i++)
  {
      setOutput(i, 1.0/getInput(i));
  }
}

void FractionNode::updateGradient()
{
  assert( getNumInputs() == getNumOutputs() );
  for(int i=0; i<getNumInputs(); i++)
  {
      const double in = getInput(i);
      notifyGradient(i, i, -1.0/(in*in));
  }
}

// Multiply node.

MultiplyNode::MultiplyNode(ComputationGraph* graph, int dim) : Node(graph, 2*dim, dim) { }

const char* MultiplyNode::name()
{
   return "Multiply";
}

void MultiplyNode::update()
{
   for(int i=0; i<getNumOutputs(); i++)
   {
      setOutput(i, getInput(i) * getInput(getNumOutputs()+i));
   }
}

void MultiplyNode::updateGradient()
{
   for(int i=0; i<getNumOutputs(); i++)
   {
      const double a = getInput(i);
      const double b = getInput(getNumOutputs()+i);
      notifyGradient(i, i, b);
      notifyGradient(getNumOutputs()+i, i, a);
   }
}
