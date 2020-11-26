#include "ERenderer.h"
#include "EStructures.h"

QELEMENT_REGISTER(ERenderer)

void ERenderer::init(const std::map<std::string, std::any>& properties, const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    out_pads.at("out")->send_buffer(in_pads.at("in")->get_buffer());
}

void ERenderer::process(const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer.get_buffer("image"));
    auto objects = std::any_cast<std::list<EObjectInfo>>(buffer.get_buffer("objects"));
    for (auto& obj : objects)
    {
        cv::rectangle(image, obj.bbox, cv::Scalar(0, 0, 255));

        int baseline;
        auto size = cv::getTextSize(obj.label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
        int top = size.height;
        cv::putText(image, obj.label + " " + std::to_string(obj.id), cv::Point(obj.bbox.x, obj.bbox.y + top), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0));

        for (auto& item : obj.properties)
        {
            top += size.height + baseline;
            auto text = item.first + ": " + item.second;
            cv::putText(image, text, cv::Point(obj.bbox.x, obj.bbox.y + top), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0));
            size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
        }
    }
    buffer.remove_buffer("objects");
    out_pads.at("out")->send_buffer(std::move(buffer));
}

void ERenderer::finalize()
{
}