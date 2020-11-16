#include "QBuffer.h"
#include <assert.h>

std::any _QBuffer::get_buffer(std::string name)
{
    return _data.at(name);
}

void _QBuffer::set_buffer(std::string name, std::any buffer)
{
    _data[name] = buffer;
}

void _QBuffer::remove_buffer(std::string name)
{
    _data.erase(name);
}