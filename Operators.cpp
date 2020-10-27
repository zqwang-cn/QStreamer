#include "Operators.h"

void Plus::init()
{
    in1 = (float*)find_in_pad("in1")->get_buffer().get();
    in2 = (float*)find_in_pad("in2")->get_buffer().get();
    out1 = (float*)find_out_pad("out1")->get_buffer().get();
}

void Plus::process()
{
    *out1 = *in1 + *in2;
}

void Minus::init()
{
    in1 = (float*)find_in_pad("in1")->get_buffer().get();
    in2 = (float*)find_in_pad("in2")->get_buffer().get();
    out1 = (float*)find_out_pad("out1")->get_buffer().get();
}

void Minus::process()
{
    *out1 = *in1 - *in2;
}

void Multiply::init()
{
    in1 = (float*)find_in_pad("in1")->get_buffer().get();
    in2 = (float*)find_in_pad("in2")->get_buffer().get();
    out1 = (float*)find_out_pad("out1")->get_buffer().get();
}

void Multiply::process()
{
    *out1 = *in1 * *in2;
}

void Divide::init()
{
    in1 = (float*)find_in_pad("in1")->get_buffer().get();
    in2 = (float*)find_in_pad("in2")->get_buffer().get();
    out1 = (float*)find_out_pad("out1")->get_buffer().get();
}

void Divide::process()
{
    *out1 = *in1 / *in2;
}
