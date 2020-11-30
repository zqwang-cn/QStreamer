#include "MDetCenterNet.h"
#include "utils.h"

MDETECTOR_REGISTER(MDetCenterNet)

MDetCenterNet::MDetCenterNet(std::string config_file) : MDetector(config_file)
{
    hm_binding_index = runtime->get_engine()->getBindingIndex(config["hm_binding_name"].asCString());
    //hmax_binding_index = runtime->get_engine()->getBindingIndex(config["hmax_binding_name"].asCString());
    wh_binding_index = runtime->get_engine()->getBindingIndex(config["wh_binding_name"].asCString());
    reg_binding_index = runtime->get_engine()->getBindingIndex(config["reg_binding_name"].asCString());

    output_height = runtime->get_engine()->getBindingDimensions(hm_binding_index).d[2];
    output_width = runtime->get_engine()->getBindingDimensions(hm_binding_index).d[3];
}

std::list<DetectionResult> MDetCenterNet::postprocess()
{
    float* hm_buf = (float*)runtime->get_buf(hm_binding_index);
    //float* hmax_buf = (float*)runtime->get_buf(hmax_binding_index);
    float* wh_buf = (float*)runtime->get_buf(wh_binding_index);
    float* reg_buf = (float*)runtime->get_buf(reg_binding_index);
    std::list<DetectionResult> results;

    // for each category
    for (int c = 0; c < n_categories; c++)
    {
        // collect box with score > threshold
        std::vector<cv::Rect2f> rect_src;
        std::vector<float> scores_src;
        std::vector<int> indices_res;
        float& threshold = thresholds[c];
        for (int y = 0; y < output_height; y++)
            for (int x = 0; x < output_width; x++)
            {
                float score = hm_buf[c * output_height * output_width + y * output_width + x];
                //float hmax = hmax_buf[c * output_height * output_width + y * output_width + x];
                //if (score == hmax && score > threshold)
                if (score > threshold)
                {
                    float& w = wh_buf[(2 * c + 0) * output_height * output_width + y * output_width + x];
                    float& h = wh_buf[(2 * c + 1) * output_height * output_width + y * output_width + x];
                    float& regx = reg_buf[(2 * c + 0) * output_height * output_width + y * output_width + x];
                    float& regy = reg_buf[(2 * c + 1) * output_height * output_width + y * output_width + x];
                    rect_src.emplace_back((x + regx - w / 2) / output_width, (y + regy - h / 2) / output_height, w / output_width, h / output_height);
                    scores_src.push_back(score);
                }
            }

        // nms
        nms(rect_src, scores_src, indices_res, 0.5);

        // generate results
        for (int index : indices_res)
        {
            DetectionResultStruct* obj = new DetectionResultStruct();
            obj->bbox = bbox_float2int(rect_src[index], padded_width, padded_height, origin_width, origin_height);
            obj->score = scores_src[index];
            obj->category = c;
            results.emplace_back(obj);
        }
    }

    return results;
}