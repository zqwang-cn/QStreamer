#include "EDisplayer.h"
#include <opencv2/opencv.hpp>

QELEMENT_REGISTER(EDisplayer)

void EDisplayer::init(const std::map<std::string, std::any>& properties, const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    title = std::any_cast<std::string>(properties.at("title"));
    auto buffer = in_pads.at("in")->get_buffer();
}

void EDisplayer::process(const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer.get_buffer("image"));
    cv::imshow(title, image);
    if (cv::waitKey(1) == 27)
        quit();
}

void EDisplayer::finalize()
{
}