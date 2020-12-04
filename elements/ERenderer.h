#pragma once
#include "../qstreamer/QElement.h"

class ERenderer : public QElement
{
public:
    virtual void init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads);
    virtual void process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads);
    virtual void finalize();
};