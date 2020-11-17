#include <opencv2/opencv.hpp>

struct EObjectInfo
{
    uint id;
    cv::Rect bbox;
    std::string label;
    std::map<std::string, std::string> properties;
};