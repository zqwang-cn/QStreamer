#pragma once
#include "Element.h"
#include "RetinaFaceDetector.h"
#include <opencv2/opencv.hpp>

class RetinaFaceDetectorElement : public Element
{
public:
    virtual void init();
    virtual void process();
    virtual void finalize();

private:
    cv::Mat* in_image;
    cv::Mat* out_image;
    std::vector<DetectorResult>* result;
    RetinaFaceDetector* detector;
};