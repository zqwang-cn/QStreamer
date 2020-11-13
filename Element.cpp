#include "Element.h"
#include "DetectorElement.h"
#include "Pipeline.h"
#include "ResultRenderer.h"
#include "VideoIOElements.h"
#include <assert.h>
#include <iostream>

Element* Element::new_element(std::string type)
{
    if (type == "VideoReader")
        return new VideoReader();
    else if (type == "ImageDisplayer")
        return new ImageDisplayer();
    else if (type == "DetectorElement")
        return new DetectorElement();
    else if (type == "ResultRenderer")
        return new ResultRenderer();
    else if (type == "RTMPPushStreamElement")
        return new RTMPPushStreamElement();
    else
        assert(false);
}

Element* Element::create_element(Json::Value config)
{
    auto type = config["type"].asString();
    Element* element = Element::new_element(type);

    auto properties = config["properties"];
    for (auto name : properties.getMemberNames())
    {
        auto property = properties[name];
        auto type = property["type"].asString();
        if (type == "string")
            element->_properties[name] = property["value"].asString();
        else if (type == "int")
            element->_properties[name] = property["value"].asInt();
        else if (type == "float")
            element->_properties[name] = property["value"].asFloat();
        else
            assert(false);
    }

    auto in_pads = config["in_pads"];
    for (auto name : in_pads.getMemberNames())
    {
        InPad* in_pad = new InPad(element);
        element->_in_pads[name] = in_pad;
        element->_pad_ready.emplace(in_pad, nullptr);
    }
    auto out_pads = config["out_pads"];
    for (auto name : out_pads.getMemberNames())
    {
        OutPad* out_pad = new OutPad(element);
        element->_out_pads[name] = out_pad;
    }
    return element;
}

Pad* Element::get_in_pad(std::string name)
{
    return _in_pads[name];
}

Pad* Element::get_out_pad(std::string name)
{
    return _out_pads[name];
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

void Element::pad_ready(Pad* pad)
{
    auto iter = _pad_ready.find(pad);
    assert(iter != _pad_ready.end());
    iter->second = true;
    for (auto iter = _pad_ready.begin(); iter != _pad_ready.end(); iter++)
        if (iter->second == false)
            return;
    _pipeline->element_ready(this);
}

void Element::unready()
{
    for (auto iter = _pad_ready.begin(); iter != _pad_ready.end(); iter++)
        iter->second = false;
}

void Element::init()
{
    init(_properties);
}

void Element::process()
{
    process(_in_pads, _out_pads);
}