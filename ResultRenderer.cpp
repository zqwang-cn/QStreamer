#include "ResultRenderer.h"

void ResultRenderer::init(const std::map<std::string, std::any>& properties)
{
}

void ResultRenderer::process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    image = std::any_cast<cv::Mat>(buffer->get_buffer("image"));
    det_results = std::any_cast<std::vector<DetectorResult>>(buffer->get_buffer("det_results"));
    for (auto result : det_results)
        cv::rectangle(image, result->bbox, cv::Scalar(0, 0, 255));
    buffer->remove_buffer("det_results");
    out_pads.at("out")->send_buffer(buffer);
}

void ResultRenderer::finalize()
{
}