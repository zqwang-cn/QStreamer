#pragma once
#include "Pad.h"
#include "json/json.h"

class Pipeline;

class Element
{
public:
    static Element* new_element(std::string class_name);
    static Element* create_element(Json::Value config, Pipeline* pipeline);
    ~Element();

    Pad* get_in_pad(std::string name);
    Pad* get_out_pad(std::string name);
    int n_in_pads();
    int n_out_pads();

    void init();
    void process();
    void unready();
    virtual void init(const std::map<std::string, std::any>& properties, const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads) = 0;
    virtual void process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads) = 0;
    virtual void finalize() = 0;

    friend class InPad;

protected:
    Pipeline* _pipeline;

private:
    std::map<std::string, std::any> _properties;
    std::map<std::string, InPad*> _in_pads;
    std::map<std::string, OutPad*> _out_pads;
    std::map<Pad*, bool> _pad_ready;
    void pad_ready(Pad* pad);
};