#include "QPad.h"
#include "QElement.h"

QPad::QPad(QElement* element) : _element(element)
{
}

void QPad::link(QPad* pad)
{
    _other_pad = pad;
}

QInPad::QInPad(QElement* element) : QPad(element)
{
}

void QInPad::receive_buffer(QBuffer&& buffer)
{
    _buffer = std::move(buffer);
    _element->pad_ready(this);
}

QBuffer&& QInPad::get_buffer()
{
    return std::move(_buffer);
}

QOutPad::QOutPad(QElement* element) : QPad(element)
{
}

void QOutPad::send_buffer(QBuffer&& buffer)
{
    ((QInPad*)_other_pad)->receive_buffer(std::move(buffer));
}