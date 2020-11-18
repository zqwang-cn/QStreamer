#include "QBuffer.h"
#include <assert.h>

QBuffer::QBuffer()
{
}

QBuffer::QBuffer(QBuffer&& buffer)
{
    _data = std::move(buffer._data);
}

std::any QBuffer::get_buffer(std::string name)
{
    return _data.at(name);
}

QBuffer& QBuffer::operator=(QBuffer&& buffer)
{
    _data = std::move(buffer._data);
    return *this;
}

void QBuffer::set_buffer(std::string name, std::any buffer)
{
    _data[name] = buffer;
}

void QBuffer::remove_buffer(std::string name)
{
    _data.erase(name);
}