#pragma once
#include "MFeatureExtractor.h"
#include "QElement.h"

class EFeatureExtractor : public QElement
{
public:
    virtual void init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads);
    virtual void process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads);
    virtual void finalize();

private:
    MFeatureExtractor<float>* extractor;
};