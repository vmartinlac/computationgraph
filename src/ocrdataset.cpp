#include "ocrdataset.h"
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>

void OCRDataset::load(const char* path)
{
    assert( _examples.empty() );

    std::ifstream f(path);
    assert(f.is_open());

    std::string line;

    bool go_on = true;
    while(go_on)
    {
        if(go_on)
        {
            go_on = (f.eof() == false);
        }

        if(go_on)
        {
            std::getline(f, line);
            go_on = (line.size() == 129);
        }

        if(go_on)
        {
            go_on = ('a' <= line[0] && line[0] <= 'z');
            for(int i=1; go_on && i<129; i++)
            {
                go_on = (line[i] == '0' || line[i] == '1');
            }
        }

        if(go_on)
        {
            Example e;
            strncpy( e.data, line.c_str(), 129);
            _examples.push_back(e);
        }
    }

    f.close();
}

int OCRDataset::getNumberOfExamples()
{
    return _examples.size();
}

int OCRDataset::getInputDimension()
{
    return 128;
}

int OCRDataset::getNumberOfClasses()
{
    return 26;
}

void OCRDataset::getExample(int id, ComputationGraph::Evaluation* eval, int node, int& cl)
{
    assert(0 <= id && id < _examples.size());

    Example& e = _examples[id];

    for(int i=0; i<128; i++)
    {
        eval->setValue(
            node,
            i,
            (e.data[1+i] == '1') ? 1.0 : -1.0);
    }

    cl = e.data[0] - 'a';
}
