#pragma once
#include "json/json.h"
#include <map>

class Element;

class Link
{
public:
    static Link* create_link(Json::Value config);

    void set_to_element(Element* element);
    void notify();
    void add_buffer(std::string name, void* buffer);
    void* get_buffer();
    void* get_buffer(std::string name);

private:
    Element* _to_element;
    std::map<std::string, void*> _buffers;
};