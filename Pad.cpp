#include "Pad.h"
#include "Element.h"

Pad::Pad(Element* element) : _element(element)
{
}

void Pad::link(Pad* pad)
{
    _other_pad = pad;
}

InPad::InPad(Element* element) : Pad(element)
{
}

void InPad::receive_buffer(Buffer&& buffer)
{
    _buffer = std::move(buffer);
    _element->pad_ready(this);
}

Buffer&& InPad::get_buffer()
{
    return std::move(_buffer);
}

OutPad::OutPad(Element* element) : Pad(element)
{
}

void OutPad::send_buffer(Buffer&& buffer)
{
    ((InPad*)_other_pad)->receive_buffer(std::move(buffer));
}