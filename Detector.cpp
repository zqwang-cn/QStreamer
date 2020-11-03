#include "Detector.h"
#include "CenternetDetector.h"
#include "RetinaFaceDetector.h"

Detector* Detector::create_detector(std::string config_file)
{
    Json::Value config = load_config_file(config_file);
    std::string type = config["type"].asString();
    if (type == "RetinaFaceDetector")
        return new RetinaFaceDetector(config_file);
    else if (type == "CenternetDetector")
        return new CenternetDetector(config_file);
    else
        assert(false);
}

Detector::Detector(std::string config_file) : Model(config_file)
{
    read_json_array(labels, config["labels"]);
    n_categories = labels.size();
    min_width = config["min_width"].asInt();
    min_height = config["min_height"].asInt();
    read_json_array(thresholds, config["thresholds"]);
}

std::vector<DetectorResult> Detector::detect(const cv::Mat& image)
{
    cv::Mat copy = image.clone();
    preprocess(copy);
    infer(copy);
    std::vector<DetectorResult> objects = postprocess();
    return objects;
}

const std::vector<std::string>& Detector::get_labels()
{
    return labels;
}

int Detector::get_n_categories()
{
    return n_categories;
}

std::vector<DetectorResult> Detector::filter_results_by_area(std::vector<DetectorResult> results, std::vector<std::vector<cv::Point>> polygons, int width, int height)
{
    cv::Mat mask = cv::Mat::zeros(height, width, CV_8UC1);
    cv::fillPoly(mask, polygons, 255);

    std::vector<DetectorResult> ret;
    for (DetectorResult& r : results)
    {
        cv::Rect& bbox = r->bbox;
        int x = bbox.x + bbox.width / 2;
        int y = bbox.y + bbox.height / 2;
        if (mask.at<uchar>(y, x) != 0)
            ret.push_back(r);
    }

    return ret;
}