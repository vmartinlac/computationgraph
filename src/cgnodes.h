#pragma once

#include "cgcore.h"


class ConstantNode : public ComputationGraph::Node
{
public:

    ConstantNode(ComputationGraph* graph, int dim) : ComputationGraph::Node(graph, 0, dim) { }

protected:

    virtual void update() { }
};

/*

class Fraction : public ComputationNode
{
public:

    Fraction() : ComputationNode(1, 1) { }

protected:

    virtual void update()
    {
        const double x = getInput(0);
        const double y = 1.0 / x;
        setOutput(0, y);
    }
};


class Multiply : public ComputationNode
{
public:

    Multiply() : ComputationNode(2, 1) { }

protected:

    virtual void update()
    {
        setOutput(0, getInput(0) * getInput(1));
    }
};


class Sum : public ComputationNode
{
public:

    Sum(int dim) : _dim(dim), ComputationNode(2*dim, dim) { }

protected:

    virtual void update()
    {
        for(int i=0; i<_dim; i++)
        {
            setOutput(i, getInput(i) + getInput(_dim+i));
        }
    }

    int _dim;
};
*/
