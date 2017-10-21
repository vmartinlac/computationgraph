#pragma once

#include <algorithm>
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

    // to be called by the nodes at their creation.
    void registerNode(Node* node);

    void freeze();

    void update();

    void computeGradient(Node* node, int output_id);

private:

    struct RegisteredNode
    {
        RegisteredNode(Node* n) : node(n) { }

        Node* node;
    };

    std::vector<RegisteredNode> _nodes;
    bool _frozen;
    int _nextPass;
};

/* Node */

class ComputationGraph::Node
{
public:

    int getNumOutputs() { return _numOutputs; }

    int getNumInputs() { return _numInputs; }

    ComputationGraph* getGraph() { return _graph; }

    void connect(int input_id, ComputationGraph::Node* source, int output_id)
    {
        assert( _graph->_frozen == false );
        assert( 0 <= input_id && input_id < _numInputs );
        assert( 0 <= output_id && output_id < source->getNumOutputs() );
        assert( source->_graph == _graph );
        _inputs[input_id].source = source;
        _inputs[input_id].output_id = output_id;
    }

    bool isReady()
    {
      return std::all_of( _inputs.begin(), _inputs.end(), [] (const Input& c) { return c.isConnected(); });
    }

    void update(int pass)
    {
        if(pass != _lastPass)
        {
            _lastPass = pass;
            for(Input& conn : _inputs)
            {
                conn.source->update(pass);
            }
            update();
        }
    }

    double getOutput(int id)
    {
        assert(0 <= id && id < _numOutputs);
        return _outputs[id].value;
    }

    double getGradient(int id)
    {
        assert(0 <= id && id < _numOutputs);
        return _outputs[id].gradient;
    }

    void setZeroGradient()
    {
        for(Output& o : _outputs)
        {
            o.gradient = 0.0;
        }
    }

protected:

    Node(ComputationGraph* graph, int num_inputs, int num_outputs) :
        _graph(graph),
        _numInputs(num_inputs),
        _numOutputs(num_outputs),
        _inputs(num_inputs),
        _outputs(num_outputs),
        _lastPass(-1)
    {
        graph->registerNode(this);
    }

    virtual void update() = 0;

    double getInput(int id)
    {
        assert(0 <= id && id < _numInputs);
        Input& c = _inputs[id];
        return c.source->getOutput(c.output_id);
    }

    void setOutput(int id, double value)
    {
        assert(0 <= id && id < _numOutputs);
        _outputs[id].value = value;
    }

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
    ComputationGraph* _graph;
    int _lastPass;
};

