#include "Pipeline.h"
#include <assert.h>

Pipeline::Pipeline(Json::Value config)
{
    auto element_config = config["elements"];
    for (auto name : element_config.getMemberNames())
    {
        auto element = Element::create_element(element_config[name], this);
        _elements.emplace(name, element);
        if (element->n_in_pads() == 0)
            _input_elements.push(element);
    }

    for (auto link_config : config["links"])
    {
        auto from = link_config["from"];
        auto from_ele_name = from["element_name"].asString();
        auto from_pad_name = from["pad_name"].asString();
        auto from_ele = _elements.at(from_ele_name);
        auto from_pad = from_ele->get_out_pad(from_pad_name);

        auto to = link_config["to"];
        auto to_ele_name = to["element_name"].asString();
        auto to_pad_name = to["pad_name"].asString();
        auto to_ele = _elements.at(to_ele_name);
        auto to_pad = to_ele->get_in_pad(to_pad_name);

        from_pad->link(to_pad);
        to_pad->link(from_pad);
    }
}

Pipeline::~Pipeline()
{
    for (auto iter = _elements.begin(); iter != _elements.end(); iter++)
        delete iter->second;
}

void Pipeline::init()
{
    _ready_elements = _input_elements;
    while (!_quit && _ready_elements.size() > 0)
    {
        auto element = _ready_elements.front();
        element->init();
        element->unready();
        _ready_elements.pop();
    }
}

void Pipeline::run()
{
    while (!_quit)
    {
        _ready_elements = _input_elements;
        while (!_quit && _ready_elements.size() > 0)
        {
            auto element = _ready_elements.front();
            element->process();
            element->unready();
            _ready_elements.pop();
        }
    }
}

void Pipeline::stop()
{
    _quit = true;
}

void Pipeline::finalize()
{
    for (auto iter = _elements.begin(); iter != _elements.end(); iter++)
        iter->second->finalize();
}

void Pipeline::element_ready(Element* element)
{
    _ready_elements.push(element);
}