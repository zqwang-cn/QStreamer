#include "RetinaFaceDetectorElement.h"

void RetinaFaceDetectorElement::init()
{
    detector = new RetinaFaceDetector(get_property("config_file"));
}

void RetinaFaceDetectorElement::process(std::map<std::string, InPad*>& in_pads, std::map<std::string, OutPad*>& out_pads)
{
    auto buffer = in_pads["in"]->get_buffer();
    image = std::any_cast<cv::Mat>(buffer->get_buffer("image"));
    results = detector->detect(image);
    buffer->set_buffer("det_results", results);
    out_pads["out"]->send_buffer(buffer);
}

void RetinaFaceDetectorElement::finalize()
{
    delete detector;
}