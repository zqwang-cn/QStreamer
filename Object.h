#pragma once
#include <map>
#include <string>

class Object
{
public:
    void set_property(std::string name, std::string value);
    std::string get_property(std::string name);

protected:
    std::map<std::string, std::string> _properties;
};