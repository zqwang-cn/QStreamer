#include "EDetector.h"
#include "EStructures.h"

QELEMENT_REGISTER(EDetector)

void EDetector::init(const std::map<std::string, std::any>& properties, const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    std::string config_file = std::any_cast<std::string>(properties.at("config_file"));
    detector = MDetector::create_detector(config_file);
    labels = detector->get_labels();
    out_pads.at("out")->send_buffer(in_pads.at("in")->get_buffer());
}

void EDetector::process(const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer.get_buffer("image"));
    auto results = detector->detect(image);
    std::list<EObjectInfo> objects;
    for (auto& r : results)
    {
        auto& obj = objects.emplace_back();
        obj.bbox = r->bbox;
        obj.label = labels[r->category];
    }
    buffer.set_buffer("objects", objects);
    out_pads.at("out")->send_buffer(std::move(buffer));
}

void EDetector::finalize()
{
    delete detector;
}