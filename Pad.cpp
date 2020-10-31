#include "Pad.h"
#include "Element.h"

Pad::Pad(Element* element, PadType type) : _element(element), _type(type)
{
}

void Pad::link(Pad* pad)
{
    _other_pad = pad;
}

void Pad::receive_buffer(Buffer* buffer)
{
    _element->set_buffer(this, buffer);
}

void Pad::send_buffer(Buffer* buffer)
{
    _other_pad->receive_buffer(buffer);
}