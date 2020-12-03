#pragma once
#include "MDetector.h"
#include "QElement.h"

class EDetectorWithFeature : public QElement
{
public:
    virtual void init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads);
    virtual void process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads);
    virtual void finalize();

private:
    MDetector* detector;
    std::vector<std::string> labels;
};