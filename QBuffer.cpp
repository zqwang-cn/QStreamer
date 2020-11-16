#include "QBuffer.h"
#include <assert.h>

std::any _QBuffer::get_buffer(std::string name)
{
    auto iter = _data.find(name);
    assert(iter != _data.end());
    return iter->second;
}

void _QBuffer::set_buffer(std::string name, std::any buffer)
{
    assert(_data.find(name) == _data.end());
    _data.emplace(name, buffer);
}

void _QBuffer::remove_buffer(std::string name)
{
    auto iter = _data.find(name);
    assert(iter != _data.end());
    _data.erase(iter);
}