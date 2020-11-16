#include "ERenderer.h"
#include "MDetector.h"

void ERenderer::init(const std::map<std::string, std::any>& properties, const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    out_pads.at("out")->send_buffer(in_pads.at("in")->get_buffer());
}

void ERenderer::process(const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer->get_buffer("image"));
    auto det_results = std::any_cast<std::list<DetectionResult>>(buffer->get_buffer("det_results"));
    for (auto result : det_results)
        cv::rectangle(image, result->bbox, cv::Scalar(0, 0, 255));
    buffer->remove_buffer("det_results");
    out_pads.at("out")->send_buffer(std::move(buffer));
}

void ERenderer::finalize()
{
}