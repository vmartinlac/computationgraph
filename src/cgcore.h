#pragma once

#include <set>
#include <vector>
#include <cassert>

/* Computation graph */

class ComputationGraph
{
public:

    class Node;
    class Evaluation;

    ComputationGraph();

    ~ComputationGraph();

    void freeze();

    void update();

    void updateGradient(Node* node, int output);

    int getNumberOfNodes();

    int getNumberOfOutputs();

private:

    std::vector<Node*> _nodes;
    bool _frozen;
    int _nextPass;
};

/* Node */

class ComputationGraph::Node
{
public:

    int getNumOutputs();

    int getNumInputs();

    ComputationGraph* owner();

    void connect(int input_id, ComputationGraph::Node* source, int output_id);

    virtual const char* name() = 0;

    bool isReady();

    // to be called by ComputationGraph::update();
    void update(int pass);

    // to be called by ComputationGraph::updateGradient();
    void updateGradient(int pass);

    // to be called by ComputationGraph::updateGradient();
    void setToDifferentiate(int output);

    double getOutput(int id);

    double getGradient(int id);

    // to be called by ComputationGraph::updateGradient();
    void prepareGradientUpdate();

protected:

    Node(ComputationGraph* graph, int num_inputs, int num_outputs);

    virtual void update() = 0;

    virtual void updateGradient() = 0;

    double getInput(int id);

    void notifyGradient(int input_id, int output_id, double value);

    void setOutput(int id, double value);

private:

    struct Input
    {
        Input() : source(nullptr), output_id(-1) { }

        ComputationGraph::Node* source;
        int output_id;

        bool isConnected() const
        {
           return output_id >= 0 && source != nullptr;
        }
    };

    struct Output
    {
        Output() : value(0.0), gradient(0.0) { }
        double value;
        double gradient;
    };


    const int _numInputs;
    const int _numOutputs;
    std::vector<Input> _inputs;
    std::vector<Output> _outputs;
    std::set<Node*> _clients;
    ComputationGraph* _graph;
    int _lastPass;
};

inline int ComputationGraph::Node::getNumOutputs()
{
    return _numOutputs;
}

inline int ComputationGraph::Node::getNumInputs()
{
    return _numInputs;
}

inline ComputationGraph* ComputationGraph::Node::owner()
{
    return _graph;
}

inline double ComputationGraph::Node::getOutput(int id)
{
    assert(0 <= id && id < _numOutputs);
    return _outputs[id].value;
}

inline double ComputationGraph::Node::getGradient(int id)
{
    assert(0 <= id && id < _numOutputs);
    return _outputs[id].gradient;
}

inline double ComputationGraph::Node::getInput(int id)
{
    assert(0 <= id && id < _numInputs);
    Input& c = _inputs[id];
    return c.source->getOutput(c.output_id);
}

inline void ComputationGraph::Node::notifyGradient(int input_id, int output_id, double value)
{
    assert( 0 <= input_id && input_id < _numInputs );
    Input& in = _inputs[input_id];
    in.source->_outputs[ in.output_id ].gradient += value*_outputs[output_id].gradient;
}

inline void ComputationGraph::Node::setOutput(int id, double value)
{
    assert(0 <= id && id < _numOutputs);
    _outputs[id].value = value;
}
