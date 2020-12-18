#include "ERenderer.h"
#include "EStructures.h"

QELEMENT_REGISTER(ERenderer)

void ERenderer::init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    out_pads["out"]->send_buffer(in_pads["in"]->get_buffer());
}

void ERenderer::process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto buffer = in_pads["in"]->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer["image"]);
    auto objects = std::any_cast<std::list<EObjectInfo>>(buffer["objects"]);
    for (auto& obj : objects)
    {
        cv::rectangle(image, obj.bbox, cv::Scalar(0, 0, 255));

        int baseline;
        auto size = cv::getTextSize(obj.label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
        int top = size.height;
        cv::putText(image, obj.label, cv::Point(obj.bbox.x, obj.bbox.y + top), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255));

        for (auto& item : obj.properties)
        {
            top += size.height + baseline;
            auto text = item.first + ": " + item.second;
            cv::putText(image, text, cv::Point(obj.bbox.x, obj.bbox.y + top), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255));
            size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
        }
    }
    buffer.erase("objects");
    out_pads["out"]->send_buffer(std::move(buffer));
}

void ERenderer::finalize()
{
}