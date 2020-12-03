#pragma once
#include "QBuffer.h"

class QElement;

class QPad
{
public:
    void link(QPad* pad);

protected:
    QPad(QElement* element);
    QElement* _element;
    QPad* _other_pad;
};

class QInPad : public QPad
{
public:
    QInPad(QElement* element);
    void receive_buffer(QBuffer&& buffer);
    QBuffer&& get_buffer();

private:
    QBuffer _buffer;
};

class QOutPad : public QPad
{
public:
    QOutPad(QElement* element);
    void send_buffer(QBuffer&& buffer);
};