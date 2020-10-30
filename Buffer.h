#pragma once
#include <any>
#include <map>
#include <string>

class Buffer
{
public:
    std::any get_buffer(std::string name);
    void set_buffer(std::string name, std::any buffer);
    void remove_buffer(std::string name);

private:
    std::map<std::string, std::any> _data;
};