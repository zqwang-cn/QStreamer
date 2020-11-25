#pragma once
#include "MFeatureExtractor.h"
#include "QElement.h"

class EFeatureExtractor : public QElement
{
public:
    virtual void init(const std::map<std::string, std::any>& properties, const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads);
    virtual void process(const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads);
    virtual void finalize();

private:
    MFeatureExtractor<float>* extractor;
};