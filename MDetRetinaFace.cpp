#include "MDetRetinaFace.h"

MDETECTOR_REGISTER(MDetRetinaFace)

MDetRetinaFace::MDetRetinaFace(std::string config_file) : MDetector(config_file)
{
    // get configs
    boxes_binding_index = runtime->get_engine()->getBindingIndex(config["boxes_binding_name"].asCString());
    scores_binding_index = runtime->get_engine()->getBindingIndex(config["scores_binding_name"].asCString());
    landmarks_binding_index = runtime->get_engine()->getBindingIndex(config["landmarks_binding_name"].asCString());
    std::vector<std::vector<int>> min_sizes;
    read_json_array(min_sizes, config["min_sizes"]);
    std::vector<int> steps;
    read_json_array(steps, config["steps"]);
    read_json_array(variance, config["variance"]);
    bool clip = config["clip"].asBool();

    // calculate priors
    std::vector<std::pair<int, int>> feature_maps;
    feature_maps.reserve(steps.size());
    for (int step : steps)
        feature_maps.push_back({(input_height - 1) / step + 1, (input_width - 1) / step + 1});
    int n_priors = 0;
    for (int k = 0; k < feature_maps.size(); k++)
        n_priors += feature_maps[k].first * feature_maps[k].second * min_sizes[k].size();
    priors.create(n_priors, 4, CV_32F);
    int index = 0;
    for (int k = 0; k < feature_maps.size(); k++)
    {
        int step = steps[k];
        int f0 = feature_maps[k].first;
        int f1 = feature_maps[k].second;
        for (int i = 0; i < f0; i++)
            for (int j = 0; j < f1; j++)
                for (int min_size : min_sizes[k])
                {
                    priors.at<float>(index, 2) = float(min_size) / input_width;
                    priors.at<float>(index, 3) = float(min_size) / input_height;
                    priors.at<float>(index, 0) = (j + 0.5) * step / input_width;
                    priors.at<float>(index, 1) = (i + 0.5) * step / input_height;
                    index++;
                }
    }

    //clip
    if (clip)
    {
        priors.setTo(0, priors < 0);
        priors.setTo(1, priors > 1);
    }
}

std::list<DetectionResult> MDetRetinaFace::postprocess()
{
    //decode box
    cv::Mat loc(priors.rows, priors.cols, CV_32F, runtime->get_buf(boxes_binding_index));
    cv::Mat boxes(priors.rows, priors.cols, CV_32F);
    boxes.colRange(0, 2) = priors.colRange(0, 2) + variance[0] * loc.colRange(0, 2).mul(priors.colRange(2, 4));
    cv::exp(variance[1] * loc.colRange(2, 4), boxes.colRange(2, 4));
    boxes.colRange(2, 4) = priors.colRange(2, 4).mul(boxes.colRange(2, 4));
    boxes.colRange(0, 2) -= boxes.colRange(2, 4) / 2;
    boxes.colRange(2, 4) += boxes.colRange(0, 2);

    // clip
    boxes.setTo(0, boxes < 0);
    boxes.setTo(1, boxes > 1);

    //decode landmarks
    cv::Mat landms = cv::Mat(priors.rows, 10, CV_32F, runtime->get_buf(landmarks_binding_index)).clone();
    for (int i = 0; i < 5; i++)
        landms.colRange(i * 2, i * 2 + 2) = priors.colRange(0, 2) + variance[0] * landms.colRange(i * 2, i * 2 + 2).mul(priors.colRange(2, 4));

    // clip
    landms.setTo(0, landms < 0);
    landms.setTo(1, landms > 1);

    // collect box with score > threshold
    float* scores_buf = (float*)runtime->get_buf(scores_binding_index);
    std::vector<cv::Rect_<float>> rect_src;
    std::vector<float> scores_src;
    std::vector<int> origin_indices;
    std::vector<int> indices_res;
    float threshold = thresholds[0];
    for (int i = 0; i < boxes.rows; i++)
    {
        float& score = scores_buf[i * 2 + 1];
        if (score > threshold)
        {
            float& x1 = boxes.at<float>(i, 0);
            float& y1 = boxes.at<float>(i, 1);
            float& x2 = boxes.at<float>(i, 2);
            float& y2 = boxes.at<float>(i, 3);
            rect_src.push_back(cv::Rect_<float>(x1, y1, x2 - x1, y2 - y1));
            scores_src.push_back(score);
            origin_indices.push_back(i);
        }
    }

    // nms
    nms(rect_src, scores_src, indices_res, 0.5);

    // generate results
    std::list<DetectionResult> results;
    for (int index : indices_res)
    {
        DetectionResultWithLandmarksStruct* obj = new DetectionResultWithLandmarksStruct();
        obj->bbox = bbox_float2int(rect_src[index], padded_width, padded_height, origin_width, origin_height);
        obj->score = scores_src[index];
        obj->category = 0;
        int origin_index = origin_indices[index];
        for (int i = 0; i < 5; i++)
        {
            cv::Point2f p(landms.at<float>(origin_index, i * 2), landms.at<float>(origin_index, i * 2 + 1));
            obj->landmarks.push_back(point_float2int(p, padded_width, padded_height, origin_width, origin_height));
        }
        results.emplace_back(obj);
    }
    return results;
}