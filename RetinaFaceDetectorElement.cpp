#include "RetinaFaceDetectorElement.h"

void RetinaFaceDetectorElement::init()
{
    detector = new RetinaFaceDetector(get_property("config_file"));
}

void RetinaFaceDetectorElement::process(std::map<std::string, Pad*>& in_pads, std::map<std::string, Pad*>& out_pads)
{
    auto buffer = get_buffer("in");
    image = std::any_cast<cv::Mat>(buffer->get_buffer("image"));
    results = detector->detect(image);
    buffer->set_buffer("det_results", results);
    out_pads["out"]->send_buffer(buffer);
}

void RetinaFaceDetectorElement::finalize()
{
    delete detector;
}