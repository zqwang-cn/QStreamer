#include "MDetector.h"
#include "MDetCenterNet.h"
#include "MDetRetinaFace.h"

MDetector* MDetector::create_detector(std::string config_file)
{
    Json::Value config = load_config_file(config_file);
    std::string type = config["type"].asString();
    if (type == "MDetRetinaFace")
        return new MDetRetinaFace(config_file);
    else if (type == "MDetCenterNet")
        return new MDetCenterNet(config_file);
    else
        assert(false);
}

MDetector::MDetector(std::string config_file) : MModel(config_file)
{
    read_json_array(labels, config["labels"]);
    n_categories = labels.size();
    min_width = config["min_width"].asInt();
    min_height = config["min_height"].asInt();
    read_json_array(thresholds, config["thresholds"]);
}

std::vector<DetectionResult> MDetector::detect(const cv::Mat& image)
{
    cv::Mat copy = image.clone();
    preprocess(copy);
    infer(copy);
    std::vector<DetectionResult> objects = postprocess();
    return objects;
}

const std::vector<std::string>& MDetector::get_labels()
{
    return labels;
}

int MDetector::get_n_categories()
{
    return n_categories;
}

std::vector<DetectionResult> MDetector::filter_results_by_area(std::vector<DetectionResult> results, std::vector<std::vector<cv::Point>> polygons, int width, int height)
{
    cv::Mat mask = cv::Mat::zeros(height, width, CV_8UC1);
    cv::fillPoly(mask, polygons, 255);

    std::vector<DetectionResult> ret;
    for (DetectionResult& r : results)
    {
        cv::Rect& bbox = r->bbox;
        int x = bbox.x + bbox.width / 2;
        int y = bbox.y + bbox.height / 2;
        if (mask.at<uchar>(y, x) != 0)
            ret.push_back(r);
    }

    return ret;
}