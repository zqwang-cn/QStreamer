#pragma once
#include "MDetector.h"

class MDetRetinaFace : public MDetector
{
public:
    MDetRetinaFace(std::string config_file);

private:
    int boxes_binding_index;
    int scores_binding_index;
    int landmarks_binding_index;
    std::vector<float> variance;
    cv::Mat priors;
    virtual std::vector<DetectionResult> postprocess();
};