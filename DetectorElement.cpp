#include "DetectorElement.h"

void DetectorElement::init(const std::map<std::string, std::any>& properties)
{
    std::string config_file = std::any_cast<std::string>(properties.at("config_file"));
    detector = Detector::create_detector(config_file);
}

void DetectorElement::process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    image = std::any_cast<cv::Mat>(buffer->get_buffer("image"));
    results = detector->detect(image);
    buffer->set_buffer("det_results", results);
    out_pads.at("out")->send_buffer(buffer);
}

void DetectorElement::finalize()
{
    delete detector;
}