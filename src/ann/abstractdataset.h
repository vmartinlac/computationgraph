#pragma once

#include <vector>

class AbstractDataset
{
public:
    virtual int getNumberOfExamples() = 0;
    virtual int getInputDimension() = 0;
    virtual int getNumberOfClasses() = 0;
    virtual void getExample(int id, std::vector<double>& example, int& cl) = 0;
};

