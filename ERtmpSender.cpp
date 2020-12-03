#include "ERtmpSender.h"
#include <opencv2/opencv.hpp>

QELEMENT_REGISTER(ERtmpSender)

void ERtmpSender::init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto uri = std::any_cast<std::string>(properties["uri"]);
    auto buffer = in_pads["in"]->get_buffer();
    auto input_meta = std::any_cast<std::map<std::string, int>>(buffer["input_meta"]);
    int width = input_meta.at("width");
    int height = input_meta.at("height");
    int fps = input_meta.at("fps");
    int stride = input_meta.at("stride");
    sender = new RTMPSender(uri, width, height, fps, stride);
}

void ERtmpSender::process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto buffer = in_pads["in"]->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer["image"]);
    sender->send(image.data);
}

void ERtmpSender::finalize()
{
    delete sender;
}
