#pragma once

#include "cgcore.h"

class ConstantNode : public ComputationGraph::Node
{
public:

    ConstantNode(ComputationGraph* graph, int dim);
    void setValue(int id, double value);
    virtual const char* name();

protected:

    virtual void update();
    virtual void updateGradient();
};

class FractionNode : public ComputationGraph::Node
{
public:

    FractionNode(ComputationGraph* graph, int dim);
    virtual const char* name();

protected:

    virtual void update();
    virtual void updateGradient();
};

class MultiplyNode : public ComputationGraph::Node
{
public:

    MultiplyNode(ComputationGraph* graph, int dim);
    virtual const char* name();

protected:

    virtual void update();
    virtual void updateGradient();
};



class SumNode : public ComputationGraph::Node
{
public:

    SumNode(ComputationGraph* graph, int dim);
    virtual const char* name();

protected:

    virtual void update();
    virtual void updateGradient();
};


class PositivePartNode : public ComputationGraph::Node
{
public:

    PositivePartNode(ComputationGraph* graph, int dim);
    virtual const char* name();

protected:

    virtual void update();
    virtual void updateGradient();
};

class ScalarProductNode : public ComputationGraph::Node
{
public:

    ScalarProductNode(ComputationGraph* graph, int dim);
    virtual const char* name();

protected:

    virtual void update();
    virtual void updateGradient();
};

class SoftmaxNode : public ComputationGraph::Node
{
public:

    SoftmaxNode(ComputationGraph* graph, int dim);
    virtual const char* name();

protected:

    virtual void update();
    virtual void updateGradient();
};

