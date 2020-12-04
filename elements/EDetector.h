#pragma once
#include "../models/MDetector.h"
#include "../qstreamer/QElement.h"

class EDetector : public QElement
{
public:
    virtual void init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads);
    virtual void process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads);
    virtual void finalize();

private:
    MDetector* detector;
    std::vector<std::string> labels;
};