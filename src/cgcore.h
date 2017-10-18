#pragma once

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

    void registerNode(Node* node, int& id, int& outputoffset);

    void freeze();

private:

    struct RegisteredNode
    {
        RegisteredNode(Node* n) : node(n) { }

        Node* node;
    };

    std::vector<RegisteredNode> _nodes;
    int _numOutputs;
    bool _frozen;
};

/* Evaluation */

class ComputationGraph::Evaluation
{
public:

    Evaluation(ComputationGraph* graph);

protected:

    std::vector<double> _values;
    ComputationGraph* _graph;
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

    /*
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
    */

protected:

    Node(ComputationGraph* graph, int num_inputs, int num_outputs) :
        _numInputs(num_inputs),
        _numOutputs(num_outputs),
        _graph(graph),
        _inputs(num_inputs)
    {
        graph->registerNode(this, _id, _outputsOffset);
    }

    virtual void update() = 0;

    double getInput(int id);

    void setOutput(int id, double value);

private:

    struct Connection
    {
        Connection() : source(nullptr), output_id(-1) {}
        ComputationGraph::Node* source;
        int output_id;
    };

    const int _numInputs;
    const int _numOutputs;
    int _outputsOffset;
    int _id;
    std::vector<Connection> _inputs;
    ComputationGraph* _graph;
};

