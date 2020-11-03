#pragma once
#include "Detector.h"
#include "Element.h"
#include <opencv2/opencv.hpp>

class DetectorElement : public Element
{
public:
    virtual void init();
    virtual void process(std::map<std::string, InPad*>& in_pads, std::map<std::string, OutPad*>& out_pads);
    virtual void finalize();

private:
    cv::Mat image;
    std::vector<DetectorResult> results;
    Detector* detector;
};