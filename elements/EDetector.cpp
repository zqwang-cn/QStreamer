#include "EDetector.h"
#include "EStructures.h"

QELEMENT_REGISTER(EDetector)

void EDetector::init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    std::string config_file = std::any_cast<std::string>(properties["config_file"]);
    detector = MDetector::create_detector(config_file);
    labels = detector->get_labels();
    out_pads["out"]->send_buffer(in_pads["in"]->get_buffer());
}

void EDetector::process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto buffer = in_pads["in"]->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer["image"]);
    auto results = detector->detect(image);
    std::list<EObjectInfo> objects;
    for (auto& r : results)
    {
        auto& obj = objects.emplace_back();
        obj.bbox = r->bbox;
        obj.score = r->score;
        obj.label = labels[r->category];
    }
    buffer["objects"] = objects;
    out_pads["out"]->send_buffer(std::move(buffer));
}

void EDetector::finalize()
{
    delete detector;
}