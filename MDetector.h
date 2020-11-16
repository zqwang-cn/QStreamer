#pragma once
#include "MModel.h"
#include "utils.h"

struct DetectionResultStruct
{
    cv::Rect bbox;
    float score;
    int category;
    virtual ~DetectionResultStruct(){};
};

struct DetectionResultWithLandmarksStruct : DetectionResultStruct
{
    std::vector<cv::Point> landmarks;
};

using DetectionResult = std::shared_ptr<DetectionResultStruct>;

class MDetector : public MModel
{
public:
    static MDetector* create_detector(std::string config_file);
    MDetector(std::string config_file);
    std::list<DetectionResult> detect(const cv::Mat& image);
    const std::vector<std::string>& get_labels();
    int get_n_categories();

protected:
    int n_categories;
    int min_width;
    int min_height;
    std::vector<float> thresholds;

private:
    virtual std::list<DetectionResult> postprocess() = 0;
    std::vector<std::string> labels;
};