#include "cgnodes.h"
#include <iostream>
#include <cmath>

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

// Sum node.

SumNode::SumNode(ComputationGraph* graph, int dim) : Node(graph, 2*dim, dim) { }

const char* SumNode::name() { return "Sum"; }

void SumNode::update()
{
    for(int i=0; i<getNumOutputs(); i++)
    {
        setOutput(i, getInput(i) + getInput(getNumOutputs()+i));
    }
}

void SumNode::updateGradient()
{
    for(int i=0; i<getNumOutputs(); i++)
    {
        notifyGradient(i, i, 1.0);
        notifyGradient(i + getNumOutputs(), i, 1.0);
    }
}

// positive part node.

PositivePartNode::PositivePartNode(ComputationGraph* graph, int dim) : Node(graph, dim, dim) { }

const char* PositivePartNode::name() { return "PositivePart"; }

void PositivePartNode::update()
{
    assert( getNumOutputs() == getNumInputs() );
    for(int i=0; i<getNumOutputs(); i++)
    {
        setOutput(i, std::max(0.0, getInput(i)));
    }
}

void PositivePartNode::updateGradient()
{
    for(int i=0; i<getNumOutputs(); i++)
    {
        const double in = getInput(i);
        if(in >= 0)
        {
            notifyGradient(i, i, 1.0);
        }
    }
}

// scalar product node.

ScalarProductNode::ScalarProductNode(ComputationGraph* graph, int dim) : Node(graph, 2*dim, 1) { }

const char* ScalarProductNode::name() { return "ScalarProduct"; }

void ScalarProductNode::update()
{
    const int dim = getNumInputs() / 2;
    assert( getNumInputs() == 2*dim );
    assert( getNumOutputs() == 1 );
    double sum = 0.0;
    for(int i=0; i<dim; i++)
    {
        sum += getInput(i) * getInput(dim+i);
    }
    setOutput(0, sum);
}

void ScalarProductNode::updateGradient()
{
    const int dim = getNumInputs() / 2;
    for(int i=0; i<dim; i++)
    {
        const double a = getInput(i);
        const double b = getInput(dim+i);
        notifyGradient(i, 0, b);
        notifyGradient(dim+i, 0, a);
    }
}

// softmax node.

SoftmaxNode::SoftmaxNode(ComputationGraph* graph, int dim) : Node(graph, dim, dim) { }

const char* SoftmaxNode::name() { return "Softmax"; }

void SoftmaxNode::update()
{
    const int dim = getNumInputs();
    std::vector<double> exp(dim);
    double sum = 0.0;
    for(int i=0; i<dim; i++)
    {
        exp[i] = std::exp( getInput(i) );
        sum += exp[i];
    }

    for(int i=0; i<dim; i++)
    {
        setOutput(i, exp[i]/sum);
    }
}

void SoftmaxNode::updateGradient()
{
    const int dim = getNumInputs();
    std::vector<double> exp(dim);
    double sum = 0.0;
    for(int i=0; i<dim; i++)
    {
        exp[i] = std::exp( getInput(i) );
        sum += exp[i];
    }

    for(int i=0; i<dim; i++)
    {
        for(int j=0; j<dim; j++)
        {
            double value = -exp[i] * exp[j] / (sum*sum);
            if(i == j)
            {
                value += exp[i] / sum;
            }
            notifyGradient(i, j, value);
        }
    }
}

// select node.

SelectNode::SelectNode(ComputationGraph* graph, int dim) : Node(graph, dim, 1), _selection(0) { }

const char* SelectNode::name() { return "SelectNode"; }

void SelectNode::update()
{
    setOutput(0, getInput(_selection));
}

void SelectNode::updateGradient()
{
    notifyGradient(_selection, 0, 1.0);
}

void SelectNode::select(int id)
{
    assert( 0 <= id && id < getNumInputs() );
    _selection = id;
}
