#include "ERegionFilter.h"
#include "EStructures.h"

QELEMENT_REGISTER(ERegionFilter)

void ERegionFilter::init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto buffer = in_pads["in"]->get_buffer();
    auto input_meta = std::any_cast<std::map<std::string, int>>(buffer["input_meta"]);
    int width = input_meta.at("width");
    int height = input_meta.at("height");
    mask = cv::Mat::zeros(height, width, CV_8UC1);

    auto polygons = std::any_cast<Json::Value>(properties["polygons"]);
    std::vector<std::vector<cv::Point>> cv_polygons(polygons.size());
    for (int i = 0; i < polygons.size(); i++)
    {
        auto& polygon = polygons[i];
        auto& cv_polygon = cv_polygons[i];
        cv_polygon.resize(polygon.size());
        for (int j = 0; j < polygon.size(); j++)
        {
            auto& point = polygon[j];
            auto& cv_point = cv_polygon[j];
            cv_point.x = point[0].asInt();
            cv_point.y = point[1].asInt();
        }
    }
    cv::fillPoly(mask, cv_polygons, 255);

    out_pads["out"]->send_buffer(std::move(buffer));
}

void ERegionFilter::process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto buffer = in_pads["in"]->get_buffer();
    auto objects = std::any_cast<std::list<EObjectInfo>>(buffer["objects"]);

    objects.remove_if([this](EObjectInfo& obj) {
        int x = obj.bbox.x + obj.bbox.width / 2;
        int y = obj.bbox.y + obj.bbox.height / 2;
        if (this->mask.at<uchar>(y, x) == 0)
            return true;
        else
            return false;
    });

    buffer["objects"] = objects;
    out_pads["out"]->send_buffer(std::move(buffer));
}

void ERegionFilter::finalize()
{
}