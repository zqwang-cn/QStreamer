#pragma once
#include "../qstreamer/QElement.h"
#include "../utils/RTMPSender.h"

class ERtmpSender : public QElement
{
public:
    virtual void init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads);
    virtual void process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads);
    virtual void finalize();

private:
    RTMPSender* sender;
};