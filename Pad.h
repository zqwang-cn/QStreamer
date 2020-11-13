#pragma once
#include "Buffer.h"

class Element;

class Pad
{
public:
    Pad(Element* element);
    void link(Pad* pad);

protected:
    Element* _element;
    Pad* _other_pad;
};

class InPad : public Pad
{
public:
    InPad(Element* element);
    void receive_buffer(Buffer&& buffer);
    Buffer&& get_buffer();

private:
    Buffer _buffer;
};

class OutPad : public Pad
{
public:
    OutPad(Element* element);
    void send_buffer(Buffer&& buffer);
};