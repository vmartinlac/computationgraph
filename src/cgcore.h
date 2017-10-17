#pragma once

#include <vector>
#include <cassert>
#include <memory>

/* Computation graph */

class ComputationGraph
{
public:

    class Node;
    class Evaluation;

    ComputationGraph();

    void registerNode(std::shared_ptr<ComputationNode> node);

    void freeze();

private:

    struct RegisteredNode
    {
        RegisteredNode(std::shared_ptr<ComputationNode> n) : node(n) { }

        std::shared_ptr<ComputationNode> node;
    };

    std::vector<RegisteredNode> _nodes;
    int _numOutputs;
    bool _frozen;
};

class ComputationGraph::Evaluation
{
public:

    Evaluation(ComputationGraph* graph);

protected:

    std::vector<double> _values;
    ComputationGraph* _graph;
};

/* Abstract node */

class ComputationGraph::Node
{
public:

    int getNumOutputs() { return _numOutputs; }

    int getNumInputs() { return _numInputs; }

    // to be called only in ComputationGraph::registerNode().
    void setOutputsOffset(int offset)
    {
        _outputsOffset = offset;
    }

    // to be called only in ComputationGraph::registerNode().
    void setGraph(ComputationGraph* graph)
    {
        assert( _graph == nullptr );
        assert( graph != nullptr );
        _graph = graph;
    }

    int getOutputsOffset()
    {
        return _outputsOffset;
    }

    ComputationGraph* getGraph() { return _graph; }

    double getOutput(ComputationGraph::Evaluation* ev, int id)
    {
        assert( 0 <= id && id < _numOutputs );
        return ev->getValue(_outputsOffset + id);
    }

    void connect(int input_id, ComputationNode* source, int output_id)
    {
        assert( 0 <= input_id && input_id < _numInputs );
        assert( 0 <= output_id && output_id < source->getNumOutputs() );
        _inputs[input_id].source = source;
        _inputs[input_id].output_id = output_id;
    }

    void update(int pass)
    {
        if(pass != _lastPass)
        {
            _lastPass = pass;
            for(Connection& conn : _inputs)
            {
                conn.source->update(pass);
            }
            update();
        }
    }

protected:

    ComputationNode(
        int num_inputs,
        int num_outputs)
    {
        _graph = nullptr;
        _numInputs = num_inputs;
        _numOutputs = num_outputs;
        _inputs.resize(num_inputs);
        for(Connection& con : _inputs)
        {
            con.source = nullptr;
            con.output_id = -1;
        }
    }

    virtual void update() = 0;

    double getInput(int id)
    {
        assert( 0 <= id && id < _numInputs );
        assert( _inputs[id].source != nullptr );
        return _inputs[id].source->getOutput(_inputs[id].output_id);
    }

    void setOutput(int id, double value)
    {
        assert( 0 <= id && id < _numOutputs );
        _outputs[id] = value;
    }

private:

    struct Connection
    {
        Connection() : source(nullptr), output_id(-1) {}
        ComputationNode* source;
        int output_id;
    };

    int _numInputs;
    int _numOutputs;
    std::vector<Connection> _inputs;
    std::vector<double> _outputs;
    int _lastPass;
};

