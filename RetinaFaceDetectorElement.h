#pragma once
#include "Element.h"
#include "RetinaFaceDetector.h"
#include <opencv2/opencv.hpp>

class RetinaFaceDetectorElement : public Element
{
public:
    virtual void init();
    virtual void process(std::map<std::string, InPad*>& in_pads, std::map<std::string, OutPad*>& out_pads);
    virtual void finalize();

private:
    cv::Mat image;
    std::vector<DetectorResult> results;
    RetinaFaceDetector* detector;
};