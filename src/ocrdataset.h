#pragma once

#include "abstractdataset.h"
#include <vector>

class OCRDataset : public AbstractDataset
{
public:
    void load(const char* path);

    virtual int getNumberOfExamples();
    virtual int getInputDimension();
    virtual int getNumberOfClasses();
    virtual void getExample(int id, std::vector<double>& example, int& cl);

protected:

    struct Example
    {
        char data[129];
    };

    std::vector<Example> _examples;
};
