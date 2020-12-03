#include "QElement.h"
#include "QPipeline.h"
#include <assert.h>

static std::unique_ptr<std::map<std::string, std::function<QElement*()>>> creators;

void QElement::register_element(std::string name, std::function<QElement*()> creator)
{
    if (creators == nullptr)
        creators.reset(new std::map<std::string, std::function<QElement*()>>);
    creators->emplace(name, creator);
}

QElement* QElement::new_element(std::string type)
{
    auto creator = creators->at(type);
    return creator();
}

QElement* QElement::create_element(Json::Value config, QPipeline* pipeline)
{
    auto type = config["type"].asString();
    QElement* element = QElement::new_element(type);
    element->_pipeline = pipeline;

    auto properties = config["properties"];
    for (auto name : properties.getMemberNames())
    {
        auto property = properties[name];
        auto type = property["type"].asString();
        if (type == "string")
            element->_properties.set(name, property["value"].asString());
        else if (type == "int")
            element->_properties.set(name, property["value"].asInt());
        else if (type == "float")
            element->_properties.set(name, property["value"].asFloat());
        else if (type == "json")
            element->_properties.set(name, property["value"]);
        else
            assert(false);
    }

    auto in_pads = config["in_pads"];
    for (auto name : in_pads.getMemberNames())
    {
        QInPad* in_pad = new QInPad(element);
        element->_in_pads.set(name, in_pad);
        element->_pad_ready.emplace(in_pad, false);
    }
    auto out_pads = config["out_pads"];
    for (auto name : out_pads.getMemberNames())
    {
        QOutPad* out_pad = new QOutPad(element);
        element->_out_pads.set(name, out_pad);
    }
    return element;
}

QElement::~QElement()
{
    _in_pads.for_each([](QInPad* pad) {
        delete pad;
    });
    _out_pads.for_each([](QOutPad* pad) {
        delete pad;
    });
}

QPad* QElement::get_in_pad(std::string name)
{
    return _in_pads[name];
}

QPad* QElement::get_out_pad(std::string name)
{
    return _out_pads[name];
}

size_t QElement::n_in_pads()
{
    return _in_pads.size();
}

size_t QElement::n_out_pads()
{
    return _out_pads.size();
}

void QElement::pad_ready(QPad* pad)
{
    auto iter = _pad_ready.find(pad);
    assert(iter != _pad_ready.end());
    iter->second = true;
    for (auto iter = _pad_ready.begin(); iter != _pad_ready.end(); iter++)
        if (iter->second == false)
            return;
    _pipeline->element_ready(this);
}

void QElement::unready()
{
    for (auto iter = _pad_ready.begin(); iter != _pad_ready.end(); iter++)
        iter->second = false;
}

void QElement::init()
{
    init(_properties, _in_pads, _out_pads);
}

void QElement::process()
{
    process(_in_pads, _out_pads);
}

void QElement::quit()
{
    _pipeline->stop();
}