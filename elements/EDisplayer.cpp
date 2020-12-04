#include "EDisplayer.h"
#include <opencv2/opencv.hpp>

QELEMENT_REGISTER(EDisplayer)

void EDisplayer::init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    title = std::any_cast<std::string>(properties["title"]);
    auto buffer = in_pads["in"]->get_buffer();
}

void EDisplayer::process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto buffer = in_pads["in"]->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer["image"]);
    cv::imshow(title, image);
    if (cv::waitKey(1) == 27)
        quit();
}

void EDisplayer::finalize()
{
}