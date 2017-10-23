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

    virtual const char* name() { return "Constant"; }

protected:

    virtual void update()
    {
        ;
    }

    virtual void updateGradient()
    {
        ;
    }
};


class Fraction : public ComputationGraph::Node
{
public:

    Fraction(ComputationGraph* graph, int dim) : Node(graph, dim, dim) { }

    virtual const char* name() { return "Constant"; }

protected:

    virtual void update()
    {
        assert( getNumInputs() == getNumOutputs() );
        for(int i=0; i<getNumInputs(); i++)
        {
            setOutput(i, 1.0/getInput(i));
        }
    }

    virtual void updateGradient()
    {
        assert( getNumInputs() == getNumOutputs() );
        for(int i=0; i<getNumInputs(); i++)
        {
            const double in = getInput(i);
            notifyGradient(i, i, -1.0/(in*in));
        }
    }
};


class Multiply : public ComputationGraph::Node
{
public:

    Multiply(ComputationGraph* graph, int dim) : Node(graph, 2*dim, dim) { }

    virtual const char* name() { return "Constant"; }

protected:

    virtual void update()
    {
        for(int i=0; i<getNumOutputs(); i++)
        {
            setOutput(i, getInput(i) * getInput(getNumOutputs()+i));
        }
    }

    virtual void updateGradient()
    {
        for(int i=0; i<getNumOutputs(); i++)
        {
            const double a = getInput(i);
            const double b = getInput(getNumOutputs()+i);
            notifyGradient(i, i, b);
            notifyGradient(getNumOutputs()+i, i, a);
        }
    }
};


class Sum : public ComputationGraph::Node
{
public:

    Sum(ComputationGraph* graph, int dim) : Node(graph, 2*dim, dim) { }

    virtual const char* name() { return "Constant"; }

protected:

    virtual void update()
    {
        for(int i=0; i<getNumOutputs(); i++)
        {
            setOutput(i, getInput(i) + getInput(getNumOutputs()+i));
        }
    }

    virtual void updateGradient()
    {
        for(int i=0; i<getNumOutputs(); i++)
        {
            notifyGradient(i, i, 1.0);
            notifyGradient(i + getNumOutputs(), i, 1.0);
        }
    }
};


class PositivePart : public ComputationGraph::Node
{
public:

    PositivePart(ComputationGraph* graph, int dim) : Node(graph, dim, dim) { }

    virtual const char* name() { return "Constant"; }

protected:

    virtual void update()
    {
        assert( getNumOutputs() == getNumInputs() );
        for(int i=0; i<getNumOutputs(); i++)
        {
            setOutput(i, std::max(0.0, getInput(i)));
        }
    }

    virtual void updateGradient()
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
};


class ScalarProduct : public ComputationGraph::Node
{
public:

    ScalarProduct(ComputationGraph* graph, int dim) : Node(graph, 2*dim, 1) { }

    virtual const char* name() { return "Constant"; }

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

    virtual void updateGradient()
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
};

