#pragma once
#include "Object.h"
#include "Pad.h"
#include "json/json.h"

class Pipeline;

class Element : public Object
{
public:
    static Element* new_element(std::string class_name);
    static Element* create_element(Json::Value config);

    void add_in_pad(std::string name, Pad* in_pad);
    void add_out_pad(std::string name, Pad* out_pad);
    Pad* find_in_pad(std::string name);
    Pad* find_out_pad(std::string name);
    int n_in_pads();
    int n_out_pads();

    void set_pipeline(Pipeline* pipeline);
    void pad_ready(Pad* pad, Buffer* buffer);
    Buffer* get_buffer(std::string pad_name);
    void unready();

    virtual void init() = 0;
    virtual void process() = 0;
    virtual void finalize() = 0;

private:
    std::map<std::string, Pad*> _in_pads;
    std::map<std::string, Pad*> _out_pads;
    std::map<Pad*, Buffer*> _buffers;
    Pipeline* _pipeline;
};