#include "ResultRenderer.h"

void ResultRenderer::init()
{
    in_image = (cv::Mat*)find_in_pad("in")->get_buffer("image");
    det_results = (std::vector<DetectorResult>*)find_in_pad("in")->get_buffer("det_results");
    out_image = (cv::Mat*)find_out_pad("out")->get_buffer("image");
}

void ResultRenderer::process()
{
    *out_image = in_image->clone();
    for (auto result : *det_results)
        cv::rectangle(*out_image, result->bbox, cv::Scalar(0, 0, 255));
}

void ResultRenderer::finalize()
{
}