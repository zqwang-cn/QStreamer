#include "Pipeline.h"
#include <assert.h>

Pipeline create_pipeline(Json::Value config)
{
    Pipeline pipeline = std::make_shared<_Pipeline>();
    std::map<std::string, Element*> elements;
    std::vector<Element*> input_elements;

    auto element_config = config["elements"];
    for (auto name : element_config.getMemberNames())
    {
        auto element = Element::create_element(element_config[name]);
        element->set_pipeline(pipeline);
        elements.emplace(name, element);
        if (element->n_in_pads() == 0)
            input_elements.push_back(element);
    }

    for (auto link_config : config["links"])
    {
        auto from = link_config["from"];
        auto from_ele_name = from["element_name"].asString();
        auto from_pad_name = from["pad_name"].asString();
        auto from_ele = elements.at(from_ele_name);
        auto from_pad = from_ele->get_out_pad(from_pad_name);

        auto to = link_config["to"];
        auto to_ele_name = to["element_name"].asString();
        auto to_pad_name = to["pad_name"].asString();
        auto to_ele = elements.at(to_ele_name);
        auto to_pad = to_ele->get_in_pad(to_pad_name);

        from_pad->link(to_pad);
        to_pad->link(from_pad);
    }

    pipeline->set(std::move(elements), std::move(input_elements));
    return pipeline;
}

void _Pipeline::set(std::map<std::string, Element*>&& elements, std::vector<Element*>&& input_elements)
{
    _elements = std::move(elements);
    _input_elements = std::move(input_elements);
}

void _Pipeline::init()
{
    for (auto iter = _input_elements.begin(); iter != _input_elements.end(); iter++)
        _ready_elements.push(*iter);

    while (!_quit && _ready_elements.size() > 0)
    {
        auto element = _ready_elements.front();
        element->init();
        element->unready();
        _ready_elements.pop();
    }
}

void _Pipeline::run()
{
    while (!_quit)
    {
        for (auto iter = _input_elements.begin(); iter != _input_elements.end(); iter++)
            _ready_elements.push(*iter);

        while (!_quit && _ready_elements.size() > 0)
        {
            auto element = _ready_elements.front();
            element->process();
            element->unready();
            _ready_elements.pop();
        }
    }
}

void _Pipeline::stop()
{
    _quit = true;
}

void _Pipeline::finalize()
{
    for (auto iter = _elements.begin(); iter != _elements.end(); iter++)
        iter->second->finalize();
}

void _Pipeline::element_ready(Element* element)
{
    _ready_elements.push(element);
}