#include "DSDetection.h"

_DSDetection::_DSDetection(cv::Rect2f bbox, std::vector<float>& feature) : bbox(bbox), feature(std::move(feature))
{
}

DSDetection ds_create_detection(cv::Rect2f bbox, std::vector<float> feature)
{
    return DSDetection(new _DSDetection(bbox, feature));
}