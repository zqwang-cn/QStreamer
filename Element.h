#pragma once
#include "Link.h"
#include "Object.h"

class Pipeline;

class Element : public Object
{
public:
    static Element* new_element(std::string class_name);
    static Element* create_element(Json::Value config);

    void add_in_pad(std::string name, Link* link);
    void add_out_pad(std::string name, Link* link);
    Link* find_in_pad(std::string name);
    Link* find_out_pad(std::string name);
    int n_in_pads();
    int n_out_pads();

    void set_pipeline(Pipeline* pipeline);
    void pad_ready(Link* link);
    void notify();
    void unready();

    virtual void init() = 0;
    virtual void process() = 0;
    virtual void finalize() = 0;

private:
    std::map<std::string, Link*> _in_pads;
    std::map<Link*, bool> _in_pads_ready;
    std::map<std::string, Link*> _out_pads;
    Pipeline* _pipeline;
};