#pragma once
#include "Models.h"
#include "utils.h"

struct DetectorResultStruct
{
    cv::Rect bbox;
    float score;
    int category;
    virtual ~DetectorResultStruct(){};
};

struct DetectorResultWithLandmarksStruct : DetectorResultStruct
{
    std::vector<cv::Point> landmarks;
};

using DetectorResult = std::shared_ptr<DetectorResultStruct>;

class Detector : public Model
{
public:
    static Detector* create_detector(std::string config_file);
    Detector(std::string config_file);
    std::vector<DetectorResult> detect(const cv::Mat& image);
    const std::vector<std::string>& get_labels();
    int get_n_categories();
    static std::vector<DetectorResult> filter_results_by_area(std::vector<DetectorResult> results, std::vector<std::vector<cv::Point>> polygons, int width, int height);

protected:
    int n_categories;
    int min_width;
    int min_height;
    std::vector<float> thresholds;

private:
    virtual std::vector<DetectorResult> postprocess() = 0;
    std::vector<std::string> labels;
};