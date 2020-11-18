#pragma once
#include "QPad.h"
#include "json/json.h"
#include <functional>

class QPipeline;

class QElement
{
public:
    static void register_element(std::string name, std::function<QElement*()> creator);
    static QElement* new_element(std::string class_name);
    static QElement* create_element(Json::Value config, QPipeline* pipeline);
    ~QElement();

    QPad* get_in_pad(std::string name);
    QPad* get_out_pad(std::string name);
    int n_in_pads();
    int n_out_pads();

    void init();
    void process();
    void unready();
    virtual void init(const std::map<std::string, std::any>& properties, const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads) = 0;
    virtual void process(const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads) = 0;
    virtual void finalize() = 0;

    friend class QInPad;

protected:
    void quit();

private:
    QPipeline* _pipeline;
    std::map<std::string, std::any> _properties;
    std::map<std::string, QInPad*> _in_pads;
    std::map<std::string, QOutPad*> _out_pads;
    std::map<QPad*, bool> _pad_ready;
    void pad_ready(QPad* pad);
};

class QElementRegistration
{
public:
    QElementRegistration(const std::string name, std::function<QElement*()> creator)
    {
        QElement::register_element(name, creator);
    }
};

#define QELEMENT_REGISTER(CLASS_NAME)                                  \
    QElementRegistration CLASS_NAME##_registration(#CLASS_NAME, []() { \
        return new CLASS_NAME();                                       \
    });
