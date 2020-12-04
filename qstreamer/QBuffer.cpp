#include "QBuffer.h"
#include <assert.h>

QBuffer::QBuffer()
{
}

QBuffer::QBuffer(QBuffer&& buffer)
{
    _data = std::move(buffer._data);
}

QBuffer& QBuffer::operator=(QBuffer&& buffer)
{
    _data = std::move(buffer._data);
    return *this;
}

std::any& QBuffer::operator[](const std::string& name)
{
    return _data[name];
}

void QBuffer::erase(const std::string& name)
{
    _data.erase(name);
}