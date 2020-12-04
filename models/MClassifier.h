#pragma once
#include "MModel.h"

class MClassifier : public MModel
{
public:
    MClassifier(std::string config_file);
    int classify(const cv::Mat object);
    const std::vector<std::string>& get_labels();

protected:
    int output_binding_index;
    int n_categories;
    float threshold;

private:
    std::vector<std::string> labels;
    virtual std::pair<int, float> postprocess();
};