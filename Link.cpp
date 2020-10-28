#include "Link.h"
#include "Element.h"
#include <assert.h>
#include <opencv2/opencv.hpp>

Link* Link::create_link(Json::Value config)
{
    Link* link = new Link();
    for (auto name : config.getMemberNames())
    {
        auto link_config = config[name];
        auto type = link_config["type"].asString();
        auto size = link_config["size"].asInt();
        void* buffer;
        if (type == "float")
            buffer = new float[size];
        else if (type == "Mat")
            buffer = new cv::Mat[size];
        else
            assert(false);
        link->add_buffer(name, buffer);
    }
    return link;
}

void Link::set_to_element(Element* element)
{
    _to_element = element;
}

void Link::notify()
{
    _to_element->pad_ready(this);
}

void Link::add_buffer(std::string name, void* buffer)
{
    _buffers.emplace(name, buffer);
}

void* Link::get_buffer()
{
    assert(_buffers.size() == 1);
    return _buffers.begin()->second;
}

void* Link::get_buffer(std::string name)
{
    auto iter = _buffers.find(name);
    assert(iter != _buffers.end());
    return iter->second;
}