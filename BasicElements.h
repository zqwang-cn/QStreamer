#pragma once
#include "Element.h"

class Input : public Element
{
public:
    virtual void init();
    virtual void process();

private:
    float* out1;
    float value;
};

class Output : public Element
{
public:
    virtual void init();
    virtual void process();

private:
    float* in1;
};