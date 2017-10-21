#pragma once

#include "cgcore.h"


class ConstantNode : public ComputationGraph::Node
{
public:

    ConstantNode(ComputationGraph* graph, int dim) : Node(graph, 0, dim) { }

    void setValue(int id, double value)
    {
        setOutput(id, value);
    }

protected:

    virtual void update()
    {
        ;
    }
};


class Fraction : public ComputationGraph::Node
{
public:

    Fraction(ComputationGraph* graph, int dim) : Node(graph, dim, dim) { }

protected:

    virtual void update()
    {
        assert( getNumInputs() == getNumOutputs() );
        for(int i=0; i<getNumInputs(); i++)
        {
            setOutput(i, 1.0/getInput(i));
        }
    }
};


class Multiply : public ComputationGraph::Node
{
public:

    Multiply(ComputationGraph* graph, int dim) : Node(graph, 2*dim, dim) { }

protected:

    virtual void update()
    {
        assert( 2*getNumOutputs() == getNumInputs() );
        for(int i=0; i<getNumOutputs(); i++)
        {
            setOutput(i, getInput(i) * getInput(getNumOutputs()+i));
        }
    }
};


class Sum : public ComputationGraph::Node
{
public:

    Sum(ComputationGraph* graph, int dim) : Node(graph, 2*dim, dim) { }

protected:

    virtual void update()
    {
        assert( 2*getNumOutputs() == getNumInputs() );
        for(int i=0; i<getNumOutputs(); i++)
        {
            setOutput(i, getInput(i) + getInput(getNumOutputs()+i));
        }
    }
};


class PositivePart : public ComputationGraph::Node
{
public:

    PositivePart(ComputationGraph* graph, int dim) : Node(graph, dim, dim) { }

protected:

    virtual void update()
    {
        assert( getNumOutputs() == getNumInputs() );
        for(int i=0; i<getNumOutputs(); i++)
        {
            setOutput(i, std::max(0.0, getInput(i)));
        }
    }
};


class ScalarProduct : public ComputationGraph::Node
{
public:

    ScalarProduct(ComputationGraph* graph, int dim) : Node(graph, 2*dim, 1) { }

protected:

    virtual void update()
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
};

