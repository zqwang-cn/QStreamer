#include "Object.h"
#include <assert.h>

void Object::set_property(std::string name, std ::string value)
{
    auto iter = _properties.find(name);
    if (iter == _properties.end())
        _properties.emplace(name, value);
    else
        iter->second = value;
}

std::string Object::get_property(std::string name)
{
    auto iter = _properties.find(name);
    assert(iter != _properties.end());
    return iter->second;
}