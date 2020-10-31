#include "Pipeline.h"
#include <assert.h>

Pipeline::Pipeline(Json::Value config)
{
    auto element_config = config["elements"];
    for (auto name : element_config.getMemberNames())
    {
        auto element = Element::create_element(element_config[name]);
        element->set_pipeline(this);
        _elements.emplace(name, element);
    }

    for (auto link_config : config["links"])
    {
        auto from = link_config["from"];
        auto from_ele_name = from["element_name"].asString();
        auto from_pad_name = from["pad_name"].asString();
        auto from_ele = get_element(from_ele_name);
        auto from_pad = from_ele->get_out_pad(from_pad_name);

        auto to = link_config["to"];
        auto to_ele_name = to["element_name"].asString();
        auto to_pad_name = to["pad_name"].asString();
        auto to_ele = get_element(to_ele_name);
        auto to_pad = to_ele->get_in_pad(to_pad_name);

        from_pad->link(to_pad);
        to_pad->link(from_pad);
    }

    for (auto iter = _elements.begin(); iter != _elements.end(); iter++)
    {
        auto element = iter->second;
        if (element->n_in_pads() == 0)
            _input_elements.push_back(element);
    }
}

void Pipeline::init()
{
    for (auto iter = _elements.begin(); iter != _elements.end(); iter++)
        iter->second->init();
}

void Pipeline::run()
{
    while (!_quit)
    {
        for (auto iter = _input_elements.begin(); iter != _input_elements.end(); iter++)
            _ready_elements.push(*iter);

        while (_ready_elements.size() > 0)
        {
            auto element = _ready_elements.front();
            element->run();
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

Element* Pipeline::get_element(std::string name)
{
    auto iter = _elements.find(name);
    assert(iter != _elements.end());
    return iter->second;
}