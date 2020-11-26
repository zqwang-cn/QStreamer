#include "DSMetrics.h"

double iou_metric(const DSTrack& track, const DSDetection& det)
{
    auto rect1 = track->get_state();
    auto rect2 = det->bbox;
    auto union_ = rect2 | rect2;
    if (union_.area() < 10)
        return 0;
    auto intersection = rect1 & rect2;
    auto iou = intersection.area() / union_.area();
    if (std::isnan(iou))
        iou = 0;
    return 1 - iou;
}

double l2_metric(const DSTrack& track, const DSDetection& det)
{
    auto& f1 = track->feature;
    auto& f2 = det->feature;
    assert(f1.size() == f2.size());

    double sum = 0;
    double temp;
    for (int i = 0; i < f1.size(); i++)
    {
        temp = f1[i] - f2[i];
        sum += temp * temp;
    }
    return sqrt(sum);
}

double l2_norm(const std::vector<float>& feature)
{
    double sum = 0;
    for (auto v : feature)
        sum += v * v;
    return sqrt(sum);
}

double dot_product(const std::vector<float>& f1, const std::vector<float>& f2)
{
    double sum = 0;
    for (uint i = 0; i < f1.size(); i++)
        sum += f1[i] * f2[i];
    return sum;
}

double cosine_metric(const DSTrack& track, const DSDetection& det)
{
    auto& f1 = track->feature;
    auto& f2 = det->feature;
    assert(f1.size() == f2.size());

    double loss = 1 - (dot_product(f1, f2) / l2_norm(f1) / l2_norm(f2));
    return loss;
}

std::function<double(const DSTrack&, const DSDetection&)> ds_get_metric(std::string name)
{
    if (name == "iou")
        return iou_metric;
    else if (name == "cosine")
        return cosine_metric;
    else if (name == "l2")
        return l2_metric;
    else if (name == "none")
        return nullptr;
    else
        assert(false);
}