#include "Element.h"
#include "Pipeline.h"
#include "ResultRenderer.h"
#include "RetinaFaceDetectorElement.h"
#include "VideoIOElements.h"
#include <assert.h>
#include <iostream>

Element* Element::new_element(std::string type)
{
    if (type == "VideoReader")
        return new VideoReader();
    else if (type == "ImageDisplayer")
        return new ImageDisplayer();
    else if (type == "RetinaFaceDetectorElement")
        return new RetinaFaceDetectorElement();
    else if (type == "ResultRenderer")
        return new ResultRenderer();
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

    auto in_pads = config["in_pads"];
    for (auto name : in_pads.getMemberNames())
    {
        Pad* in_pad = new Pad(element, PadType::InPad);
        element->_in_pads[name] = in_pad;
        element->_buffers.emplace(in_pad, nullptr);
    }
    auto out_pads = config["out_pads"];
    for (auto name : out_pads.getMemberNames())
    {
        Pad* out_pad = new Pad(element, PadType::OutPad);
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

void Element::set_buffer(Pad* pad, Buffer* buffer)
{
    auto iter = _buffers.find(pad);
    assert(iter != _buffers.end());
    iter->second = buffer;
    for (auto iter = _buffers.begin(); iter != _buffers.end(); iter++)
        if (iter->second == nullptr)
            return;
    _pipeline->element_ready(this);
}

Buffer* Element::get_buffer(std::string pad_name)
{
    auto pad = _in_pads[pad_name];
    auto iter = _buffers.find(pad);
    assert(iter != _buffers.end());
    auto buffer = iter->second;
    return buffer;
}

void Element::unready()
{
    for (auto iter = _buffers.begin(); iter != _buffers.end(); iter++)
        iter->second = nullptr;
}

void Element::run()
{
    process(_in_pads, _out_pads);
}