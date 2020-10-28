#pragma once
#include "Element.h"
#include "RetinaFaceDetector.h"
#include <opencv2/opencv.hpp>

class ResultRenderer : public Element
{
public:
    virtual void init();
    virtual void process();
    virtual void finalize();

private:
    cv::Mat* in_image;
    std::vector<DetectorResult>* det_results;
    cv::Mat* out_image;
};