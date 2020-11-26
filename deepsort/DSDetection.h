#pragma once
#include <memory>
#include <opencv2/opencv.hpp>

struct _DSDetection
{
    _DSDetection(cv::Rect2f bbox, std::vector<float>& feature);
    uint id;
    cv::Rect2f bbox;
    std::vector<float> feature;
};

using DSDetection = std::shared_ptr<_DSDetection>;

DSDetection ds_create_detection(cv::Rect2f bbox, std::vector<float> feature);