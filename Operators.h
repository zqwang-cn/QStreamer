#pragma once
#include "Element.h"

class Plus : public Element
{
public:
    virtual void init();
    virtual void process();

private:
    float* in1;
    float* in2;
    float* out1;
};

class Minus : public Element
{
public:
    virtual void init();
    virtual void process();

private:
    float* in1;
    float* in2;
    float* out1;
};

class Multiply : public Element
{
public:
    virtual void init();
    virtual void process();

private:
    float* in1;
    float* in2;
    float* out1;
};

class Divide : public Element
{
public:
    virtual void init();
    virtual void process();

private:
    float* in1;
    float* in2;
    float* out1;
};