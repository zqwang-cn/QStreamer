#include "Element.h"
#include "BasicElements.h"
#include "Operators.h"
#include "Pipeline.h"
#include <assert.h>
#include <iostream>

Element* Element::new_element(std::string type)
{
    if (type == "Input")
        return new Input();
    else if (type == "Output")
        return new Output();
    else if (type == "Plus")
        return new Plus();
    else if (type == "Minus")
        return new Minus();
    else if (type == "Multiply")
        return new Multiply();
    else if (type == "Divide")
        return new Divide();
    else
        assert(false);
}

Element* Element::create_element(Json::Value config)
{
    auto type = config["type"].asString();
    Element* element = Element::new_element(type);

    auto properties = config["properties"];
    for (auto property : properties)
    {
        auto name = property["name"].asString();
        auto value = property["value"].asString();
        element->set_property(name, value);
    }

    auto out_pads = config["out_pads"];
    for (auto name : out_pads.getMemberNames())
    {
        Link* link = Link::create_link(out_pads[name]);
        element->add_out_pad(name, link);
    }
    return element;
}

void Element::add_in_pad(std::string name, Link* link)
{
    _in_pads.emplace(name, link);
    _in_pads_ready.emplace(link, false);
    link->set_to_element(this);
}

void Element::add_out_pad(std::string name, Link* link)
{
    _out_pads.emplace(name, link);
}

Link* Element::find_in_pad(std::string name)
{
    auto iter = _in_pads.find(name);
    assert(iter != _in_pads.end());
    return iter->second;
}

Link* Element::find_out_pad(std::string name)
{
    auto iter = _out_pads.find(name);
    assert(iter != _out_pads.end());
    return iter->second;
}

int Element::n_in_pads()
{
    return _in_pads.size();
}

int Element::n_out_pads()
{
    return _out_pads.size();
}

void Element::set_pipeline(Pipeline* pipeline)
{
    _pipeline = pipeline;
}

void Element::pad_ready(Link* link)
{
    auto iter = _in_pads_ready.find(link);
    assert(iter != _in_pads_ready.end());
    iter->second = true;
    for (auto iter = _in_pads_ready.begin(); iter != _in_pads_ready.end(); iter++)
        if (!iter->second)
            return;
    _pipeline->element_ready(this);
}

void Element::notify()
{
    for (auto iter = _out_pads.begin(); iter != _out_pads.end(); iter++)
        iter->second->notify();
}

void Element::unready()
{
    for (auto iter = _in_pads_ready.begin(); iter != _in_pads_ready.end(); iter++)
        iter->second = false;
}