#include "Link.h"
#include "Element.h"
#include <assert.h>

Link* Link::create_link(Json::Value config)
{
    auto type = config["type"].asString();
    auto size = config["size"].asInt();
    char* buffer;
    if (type == "float")
        buffer = (char*)(new float[size]);
    else
        assert(false);

    Link* link = new Link(buffer);
    return link;
}

Link::Link(char* ptr) : _buffer(ptr)
{
}

void Link::set_to_element(Element* element)
{
    _to_element = element;
}

void Link::notify()
{
    _to_element->pad_ready(this);
}

std::shared_ptr<char> Link::get_buffer()
{
    return _buffer;
}