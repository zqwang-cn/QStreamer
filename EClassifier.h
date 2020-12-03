#pragma once
#include "MClassifier.h"
#include "QElement.h"

class EClassifier : public QElement
{
public:
    virtual void init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads);
    virtual void process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads);
    virtual void finalize();

private:
    MClassifier* classifier;
    std::string name;
    std::vector<std::string> labels;
};