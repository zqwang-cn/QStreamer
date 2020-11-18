#include "ERtmpSender.h"
#include <opencv2/opencv.hpp>

void ERtmpSender::init(const std::map<std::string, std::any>& properties, const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    auto uri = std::any_cast<std::string>(properties.at("uri"));
    auto buffer = in_pads.at("in")->get_buffer();
    auto input_meta = std::any_cast<std::map<std::string, int>>(buffer.get_buffer("input_meta"));
    int width = input_meta.at("width");
    int height = input_meta.at("height");
    int fps = input_meta.at("fps");
    int stride = input_meta.at("stride");
    sender = new RTMPSender(uri, width, height, fps, stride);
}

void ERtmpSender::process(const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer.get_buffer("image"));
    sender->send(image.data);
}

void ERtmpSender::finalize()
{
    delete sender;
}
