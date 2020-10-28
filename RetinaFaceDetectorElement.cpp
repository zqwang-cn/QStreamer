#include "RetinaFaceDetectorElement.h"

void RetinaFaceDetectorElement::init()
{
    in_image = (cv::Mat*)find_in_pad("in")->get_buffer();
    out_image = (cv::Mat*)find_out_pad("out")->get_buffer("image");
    result = (std::vector<DetectorResult>*)find_out_pad("out")->get_buffer("det_results");
    detector = new RetinaFaceDetector(get_property("config_file"));
}

void RetinaFaceDetectorElement::process()
{
    auto r = detector->detect(*in_image);
    *out_image = *in_image;
    *result = r;
}

void RetinaFaceDetectorElement::finalize()
{
    delete detector;
}