#include "MModel.h"
#include "../utils/utils.h"

MModel::MModel(std::string config_file)
{
    // set base path
    base = std::filesystem::path(config_file);
    base.remove_filename();

    config = load_config_file(config_file);

    // create model
    std::string model_type = config["model_type"].asString();
    std::string model_file = absolute(config["model_file"].asString(), base);
    if (model_type == "CAFFE" || model_type == "caffe")
    {
        std::string proto_file = absolute(config["proto_file"].asString(), base);
        std::vector<std::string> output_names;
        for (Json::Value output_name : config["output_names"])
            output_names.push_back(output_name.asString());
        runtime = new TRTRuntime(TRTModelType::CAFFE, model_file, proto_file, output_names);
    }
    else if (model_type == "ONNX" || model_type == "onnx")
        runtime = new TRTRuntime(TRTModelType::ONNX, model_file);
    else if (model_type == "UFF" || model_type == "uff")
        runtime = new TRTRuntime(TRTModelType::UFF, model_file);
    else
    {
        std::cerr << "wrong model type" << std::endl;
        return;
    }

    for (int i = 0; i < runtime->get_engine()->getNbBindings(); i++)
        if (runtime->get_engine()->bindingIsInput(i))
        {
            input_binding_index = i;
            break;
        }
    input_height = runtime->get_engine()->getBindingDimensions(input_binding_index).d[2];
    input_width = runtime->get_engine()->getBindingDimensions(input_binding_index).d[3];

    name = config["name"].asString();
    keep_aspect_ratio = config["keep_aspect_ratio"].asBool();
    color_mode = config["color_mode"].asString();
    transform(color_mode.begin(), color_mode.end(), color_mode.begin(), ::toupper);
    std::vector<float> rgb;
    read_json_array(rgb, config["mean"]);
    mean = cv::Scalar(rgb[0], rgb[1], rgb[2]);
    read_json_array(rgb, config["std"]);
    std = cv::Scalar(rgb[0], rgb[1], rgb[2]);
}

MModel::~MModel()
{
    delete runtime;
}

void MModel::copy_data(void *src_buf, void *dst_buf, int width, int height, int n_channels)
{
    float *src = (float *)src_buf;
    float *dst = (float *)dst_buf;
    for (int c = 0; c < n_channels; c++)
        for (int x = 0; x < height; x++)
            for (int y = 0; y < width; y++)
                dst[c * height * width + x * width + y] = src[x * width * n_channels + y * n_channels + c];
}

void MModel::preprocess(cv::Mat &image)
{
    if (keep_aspect_ratio)
    {
        origin_width = image.cols;
        origin_height = image.rows;
        std::pair<int, int> size = resize_keeping_aspect_ratio(image, image, cv::Size(input_width, input_height));
        padded_width = size.first;
        padded_height = size.second;
    }
    else
        cv::resize(image, image, cv::Size(input_width, input_height));

    if (color_mode == "RGB")
        cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
    image.convertTo(image, CV_32FC3);
    std::vector<cv::Mat> channels;
    cv::split(image, channels);
    for (int i = 0; i < 3; i++)
        channels[i] = (channels[i] - mean[i]) / std[i];
    cv::merge(channels, image);
}

void MModel::infer(cv::Mat &image)
{
    copy_data(image.data, runtime->get_buf(input_binding_index), image.cols, image.rows, image.channels());
    runtime->execute();
}

const std::string &MModel::get_name()
{
    return name;
}

int MModel::get_input_width()
{
    return input_width;
}

int MModel::get_input_height()
{
    return input_height;
}