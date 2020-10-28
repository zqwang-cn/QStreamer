#include "BasicElements.h"

void Input::init()
{
    out1 = (float*)find_out_pad("out1")->get_buffer().get();
    value = std::stof(get_property("value"));
}

void Input::process()
{
    *out1 = value;
}

void Input::finalize()
{
}

void Output::init()
{
    in1 = (float*)find_in_pad("in1")->get_buffer().get();
}

void Output::process()
{
    printf("%f\n", *in1);
}

void Output::finalize()
{
}