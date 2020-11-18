#include "MDetector.h"

static std::unique_ptr<std::map<std::string, std::function<MDetector*(std::string)>>> creators;

void MDetector::register_detector(std::string name, std::function<MDetector*(std::string)> creator)
{
    if (creators == nullptr)
        creators.reset(new std::map<std::string, std::function<MDetector*(std::string)>>);
    creators->emplace(name, creator);
}

MDetector* MDetector::create_detector(std::string config_file)
{
    Json::Value config = load_config_file(config_file);
    std::string type = config["type"].asString();
    auto creator = creators->at(type);
    return creator(config_file);
}

MDetector::MDetector(std::string config_file) : MModel(config_file)
{
    read_json_array(labels, config["labels"]);
    n_categories = labels.size();
    min_width = config["min_width"].asInt();
    min_height = config["min_height"].asInt();
    read_json_array(thresholds, config["thresholds"]);
}

std::list<DetectionResult> MDetector::detect(const cv::Mat& image)
{
    cv::Mat copy = image.clone();
    preprocess(copy);
    infer(copy);
    std::list<DetectionResult> objects = postprocess();
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