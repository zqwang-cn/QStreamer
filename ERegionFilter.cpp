#include "ERegionFilter.h"
#include "EStructures.h"

void ERegionFilter::init(const std::map<std::string, std::any>& properties, const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    auto input_meta = std::any_cast<std::map<std::string, int>>(buffer->get_buffer("input_meta"));
    int width = input_meta.at("width");
    int height = input_meta.at("height");
    mask = cv::Mat::zeros(height, width, CV_8UC1);

    auto polygons = std::any_cast<Json::Value>(properties.at("polygons"));
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

    out_pads.at("out")->send_buffer(std::move(buffer));
}

void ERegionFilter::process(const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    auto objects = std::any_cast<std::list<EObjectInfo>>(buffer->get_buffer("objects"));

    objects.remove_if([this](EObjectInfo& obj) {
        int x = obj.bbox.x + obj.bbox.width / 2;
        int y = obj.bbox.y + obj.bbox.height / 2;
        if (this->mask.at<uchar>(y, x) == 0)
            return true;
        else
            return false;
    });

    buffer->set_buffer("objects", objects);
    out_pads.at("out")->send_buffer(std::move(buffer));
}

void ERegionFilter::finalize()
{
}