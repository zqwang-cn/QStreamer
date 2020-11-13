#pragma once
#include "Element.h"
#include "RetinaFaceDetector.h"
#include <opencv2/opencv.hpp>

class ResultRenderer : public Element
{
public:
    virtual void init(const std::map<std::string, std::any>& properties);
    virtual void process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads);
    virtual void finalize();

private:
    cv::Mat image;
    std::vector<DetectorResult> det_results;
};