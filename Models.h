#pragma once
#include <opencv2/opencv.hpp>
#include <filesystem>
#include "json/json.h"
#include "TRTRuntime.h"

class Model
{
public:
    Model(std::string config_file);
    ~Model();
    const std::string &get_name();
    int get_input_width();
    int get_input_height();

protected:
    Json::Value config;
    TRTRuntime *runtime;
    int input_binding_index;
    int input_width;
    int input_height;
    bool keep_aspect_ratio;
    int origin_width;
    int origin_height;
    int padded_width;
    int padded_height;
    std::string color_mode;
    cv::Scalar mean;
    cv::Scalar std;

    void preprocess(cv::Mat &image);
    void infer(cv::Mat &image);

private:
    std::string name;
    std::filesystem::path base;
    void copy_data(void *src_buf, void *dst_buf, int width, int height, int n_channels);
};