#pragma once
#include "QElement.h"
#include "RTMPSender.h"

class ERtmpSender : public QElement
{
public:
    virtual void init(const std::map<std::string, std::any>& properties, const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads);
    virtual void process(const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads);
    virtual void finalize();

private:
    RTMPSender* sender;
};