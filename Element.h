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

    Pad* get_in_pad(std::string name);
    Pad* get_out_pad(std::string name);
    int n_in_pads();
    int n_out_pads();

    void set_pipeline(Pipeline* pipeline);
    void set_buffer(Pad* pad, Buffer* buffer);
    Buffer* get_buffer(std::string pad_name);
    void unready();

    void run();
    virtual void init() = 0;
    virtual void process(std::map<std::string, Pad*>& in_pads, std::map<std::string, Pad*>& out_pads) = 0;
    virtual void finalize() = 0;

private:
    std::map<std::string, Pad*> _in_pads;
    std::map<std::string, Pad*> _out_pads;
    std::map<Pad*, Buffer*> _buffers;
    Pipeline* _pipeline;
};