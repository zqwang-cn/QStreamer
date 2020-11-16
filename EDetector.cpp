#include "EDetector.h"

void EDetector::init(const std::map<std::string, std::any>& properties, const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    std::string config_file = std::any_cast<std::string>(properties.at("config_file"));
    detector = MDetector::create_detector(config_file);
    out_pads.at("out")->send_buffer(in_pads.at("in")->get_buffer());
}

void EDetector::process(const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer->get_buffer("image"));
    auto results = detector->detect(image);
    buffer->set_buffer("det_results", results);
    out_pads.at("out")->send_buffer(std::move(buffer));
}

void EDetector::finalize()
{
    delete detector;
}