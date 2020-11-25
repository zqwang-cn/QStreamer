#pragma once
#include "MModel.h"

template <typename T>
class MFeatureExtractor : public MModel
{
public:
    MFeatureExtractor(std::string config_file);
    std::vector<T> extract(const cv::Mat &image);
    int get_feature_num();

private:
    int feature_binding_index;
    int feature_num;
};

template <typename T>
MFeatureExtractor<T>::MFeatureExtractor(std::string config_file) : MModel(config_file)
{
    for (int i = 0; i < runtime->get_engine()->getNbBindings(); i++)
        if (!runtime->get_engine()->bindingIsInput(i))
        {
            feature_binding_index = i;
            break;
        }
    feature_num = runtime->get_engine()->getBindingDimensions(feature_binding_index).d[1];
    assert(runtime->get_buf_size(feature_binding_index) == feature_num * sizeof(T));
}

template <typename T>
std::vector<T> MFeatureExtractor<T>::extract(const cv::Mat &image)
{
    cv::Mat copy = image.clone();
    preprocess(copy);
    infer(copy);

    std::vector<T> feature(feature_num);
    memcpy(feature.data(), runtime->get_buf(feature_binding_index), feature_num * sizeof(T));
    return feature;
}

template <typename T>
int MFeatureExtractor<T>::get_feature_num()
{
    return feature_num;
}