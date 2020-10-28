#pragma once
#include "Detector.h"

class RetinaFaceDetector : public Detector
{
public:
    RetinaFaceDetector(std::string config_file);

private:
    int boxes_binding_index;
    int scores_binding_index;
    int landmarks_binding_index;
    std::vector<float> variance;
    cv::Mat priors;
    virtual std::vector<DetectorResult> postprocess();
};