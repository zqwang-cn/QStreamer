#pragma once
#include "MModel.h"

class MKeypointDet2D106 : public MModel
{
public:
    MKeypointDet2D106(std::string config_file);
    std::vector<cv::Point> detect(const cv::Mat& image);

private:
    int output_binding_index;
    int n_points;
};