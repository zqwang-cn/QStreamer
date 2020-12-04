#include "QPipeline.h"
#include <assert.h>

QPipeline::QPipeline(Json::Value config)
{
    auto element_config = config["elements"];
    for (auto name : element_config.getMemberNames())
    {
        auto element = QElement::create_element(element_config[name], this);
        _elements.set(name, element);
        if (element->n_in_pads() == 0)
            _input_elements.push(element);
    }

    for (auto link_config : config["links"])
    {
        auto from = link_config["from"];
        auto from_ele_name = from["element_name"].asString();
        auto from_pad_name = from["pad_name"].asString();
        auto from_ele = _elements[from_ele_name];
        auto from_pad = from_ele->get_out_pad(from_pad_name);

        auto to = link_config["to"];
        auto to_ele_name = to["element_name"].asString();
        auto to_pad_name = to["pad_name"].asString();
        auto to_ele = _elements[to_ele_name];
        auto to_pad = to_ele->get_in_pad(to_pad_name);

        from_pad->link(to_pad);
        to_pad->link(from_pad);
    }
}

QPipeline::~QPipeline()
{
    _elements.for_each([](QElement* element) {
        delete element;
    });
}

void QPipeline::init()
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

void QPipeline::run()
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

void QPipeline::stop()
{
    _quit = true;
}

void QPipeline::finalize()
{
    _elements.for_each([](QElement* element) {
        element->finalize();
    });
}

void QPipeline::element_ready(QElement* element)
{
    _ready_elements.push(element);
}