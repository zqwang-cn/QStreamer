#pragma once
#include "Buffer.h"

enum class PadType
{
    InPad,
    OutPad
};

class Element;

class Pad
{
public:
    Pad(Element* element, PadType type);
    void link(Pad* pad);
    void send_buffer(Buffer* buffer);
    void receive_buffer(Buffer* buffer);

private:
    Element* _element;
    PadType _type;
    Pad* _other_pad;
};