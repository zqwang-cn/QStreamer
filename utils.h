#pragma once
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include "json/json.h"

inline void nms(
    const std::vector<cv::Rect_<float>> &srcRects,
    const std::vector<float> &scores,
    std::vector<int> &resIndices,
    float thresh,
    int neighbors = 0,
    float minScoresSum = 0.f)
{
    resIndices.clear();

    const size_t size = srcRects.size();
    if (!size)
    {
        return;
    }

    assert(srcRects.size() == scores.size());

    // Sort the bounding boxes by the detection score
    std::multimap<float, size_t> idxs;
    for (size_t i = 0; i < size; ++i)
    {
        idxs.insert(std::pair<float, size_t>(scores[i], i));
    }

    // keep looping while some indexes still remain in the indexes list
    while (idxs.size() > 0)
    {
        // grab the last rectangle
        auto lastElem = --std::end(idxs);
        const cv::Rect_<float> &rect1 = srcRects[lastElem->second];
        const int index = lastElem->second;

        int neigborsCount = 0;
        float scoresSum = lastElem->first;

        idxs.erase(lastElem);

        for (auto pos = std::begin(idxs); pos != std::end(idxs);)
        {
            // grab the current rectangle
            const cv::Rect_<float> &rect2 = srcRects[pos->second];

            float intArea = (rect1 & rect2).area();
            float unionArea = rect1.area() + rect2.area() - intArea;
            float overlap = intArea / unionArea;

            // if there is sufficient overlap, suppress the current bounding box
            if (overlap > thresh)
            {
                scoresSum += pos->first;
                pos = idxs.erase(pos);
                ++neigborsCount;
            }
            else
            {
                ++pos;
            }
        }
        if (neigborsCount >= neighbors && scoresSum >= minScoresSum)
        {
            resIndices.push_back(index);
        }
    }
}

inline std::pair<int, float> argmax(float *numbers, int total)
{
    assert(total > 0);
    int index = 0;
    for (int i = 1; i < total; i++)
        if (numbers[i] > numbers[index])
            index = i;
    return std::pair<int, float>(index, numbers[index]);
}

// load json format config file
inline Json::Value load_config_file(const std::string &filename)
{
    assert(std::filesystem::exists(filename));
    Json::Value config;
    std::ifstream config_file(filename);
    config_file >> config;
    config_file.close();
    return config;
}

// convert relative path to absolute path assuming currently in base
inline std::filesystem::path absolute(std::filesystem::path path, std::filesystem::path base)
{
    if (path.is_absolute())
        return path;
    std::filesystem::path ret = std::filesystem::absolute(base);
    ret += path;
    ret = std::filesystem::canonical(ret);
    return ret;
}

inline std::string absolute(std::string path_str, std::filesystem::path base)
{
    std::filesystem::path path(path_str);
    if (path.is_absolute())
        return path_str;
    std::filesystem::path ret = std::filesystem::absolute(base);
    ret += path;
    //ret = std::filesystem::canonical(ret);
    return ret.string();
}

inline void read_json_array(std::vector<int> &ret, Json::Value array)
{
    int n = array.size();
    ret.resize(n);
    for (int i = 0; i < n; i++)
        ret[i] = array[i].asInt();
}

inline void read_json_array(std::vector<float> &ret, Json::Value array)
{
    int n = array.size();
    ret.resize(n);
    for (int i = 0; i < n; i++)
        ret[i] = array[i].asFloat();
}

inline void read_json_array(std::vector<std::string> &ret, Json::Value array)
{
    int n = array.size();
    ret.resize(n);
    for (int i = 0; i < n; i++)
        ret[i] = array[i].asString();
}

inline void read_json_array(std::vector<std::vector<int>> &ret, Json::Value array)
{
    int n = array.size();
    ret.resize(n);
    for (int i = 0; i < n; i++)
        read_json_array(ret[i], array[i]);
}

inline std::pair<int, int> resize_keeping_aspect_ratio(cv::Mat input, cv::Mat &output, cv::Size size)
{
    int input_w = input.cols;
    int input_h = input.rows;
    int output_w = size.width;
    int output_h = size.height;
    float input_ratio = float(input_w) / input_h;
    float output_ratio = float(output_w) / output_h;
    float ratio;
    int temp_w, temp_h;
    if (input_ratio > output_ratio)
    {
        temp_w = input_w;
        temp_h = int(input_w / output_ratio);
        ratio = float(output_w) / input_w;
    }
    else
    {
        temp_w = int(input_h * output_ratio);
        temp_h = input_h;
        ratio = float(output_h) / input_h;
    }
    cv::Mat temp(temp_h, temp_w, input.type(), cv::Scalar(0, 0, 0));
    input.copyTo(temp.rowRange(0, input_h).colRange(0, input_w));
    cv::resize(temp, output, size);
    return {temp_w, temp_h};
}

// convert float bbox to int
inline cv::Rect bbox_float2int(cv::Rect2f bbox_float, int width, int height, int clip_width = -1, int clip_height = -1)
{
    cv::Rect bbox_int(bbox_float.x * width, bbox_float.y * height, bbox_float.width * width, bbox_float.height * height);
    // clip
    if (clip_width != -1 && clip_height != -1)
    {
        bbox_int.x = bbox_int.x < 0 ? 0 : bbox_int.x;
        bbox_int.y = bbox_int.y < 0 ? 0 : bbox_int.y;
        bbox_int.width = bbox_int.x + bbox_int.width > clip_width ? clip_width - bbox_int.x : bbox_int.width;
        bbox_int.height = bbox_int.y + bbox_int.height > clip_height ? clip_height - bbox_int.y : bbox_int.height;
    }
    return bbox_int;
}

// convert float point to int
inline cv::Point point_float2int(cv::Point2f point_float, int width, int height, int clip_width = -1, int clip_height = -1)
{
    cv::Point point_int(point_float.x * width, point_float.y * height);
    // clip
    if (clip_width != -1 && clip_height != -1)
    {
        point_int.x = point_int.x < 0 ? 0 : point_int.x;
        point_int.y = point_int.y < 0 ? 0 : point_int.y;
    }
    return point_int;
}