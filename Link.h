#pragma once
#include "json/json.h"

class Element;

class Link
{
public:
    static Link* create_link(Json::Value config);

    Link(char* ptr);
    void set_to_element(Element* element);
    void notify();
    std::shared_ptr<char> get_buffer();

private:
    Element* _to_element;
    std::shared_ptr<char> _buffer;
};