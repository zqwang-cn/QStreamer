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
    std::vector<DetectionResult> detect(const cv::Mat& image);
    const std::vector<std::string>& get_labels();
    int get_n_categories();
    static std::vector<DetectionResult> filter_results_by_area(std::vector<DetectionResult> results, std::vector<std::vector<cv::Point>> polygons, int width, int height);

protected:
    int n_categories;
    int min_width;
    int min_height;
    std::vector<float> thresholds;

private:
    virtual std::vector<DetectionResult> postprocess() = 0;
    std::vector<std::string> labels;
};