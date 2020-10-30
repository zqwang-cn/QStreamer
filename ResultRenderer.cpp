#include "ResultRenderer.h"

void ResultRenderer::init()
{
}

void ResultRenderer::process()
{
    auto buffer = get_buffer("in");
    image = std::any_cast<cv::Mat>(buffer->get_buffer("image"));
    det_results = std::any_cast<std::vector<DetectorResult>>(buffer->get_buffer("det_results"));
    for (auto result : det_results)
        cv::rectangle(image, result->bbox, cv::Scalar(0, 0, 255));
    buffer->remove_buffer("det_results");
    find_out_pad("out")->send_buffer(buffer);
}

void ResultRenderer::finalize()
{
}