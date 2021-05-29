#include "MKeypointDet2D106.h"

MKeypointDet2D106::MKeypointDet2D106(std::string config_file) : MModel(config_file)
{
    for (int i = 0; i < runtime->get_engine()->getNbBindings(); i++)
        if (!runtime->get_engine()->bindingIsInput(i))
        {
            output_binding_index = i;
            break;
        }
    n_points = runtime->get_buf_size(output_binding_index) / sizeof(float) / 2;
}

std::vector<cv::Point> MKeypointDet2D106::detect(const cv::Mat& image)
{
    cv::Mat copy = image.clone();
    preprocess(copy);
    infer(copy);

    std::vector<cv::Point> result(n_points);
    float* buf = (float*)runtime->get_buf(output_binding_index);
    for (int i = 0; i < n_points; i++)
    {
        auto& p = result[i];
        p.x = (buf[i * 2 + 0] + 1) / 2 * origin_width;
        p.y = (buf[i * 2 + 1] + 1) / 2 * origin_height;
    }

    return result;
}