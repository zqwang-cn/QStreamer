#include "MClassifier.h"
#include "utils.h"

MClassifier::MClassifier(std::string config_file) : MModel(config_file)
{
    threshold = config["threshold"].asFloat();
    read_json_array(labels, config["labels"]);
    for (int i = 0; i < runtime->get_engine()->getNbBindings(); i++)
        if (!runtime->get_engine()->bindingIsInput(i))
        {
            output_binding_index = i;
            break;
        }
    n_categories = runtime->get_engine()->getBindingDimensions(output_binding_index).d[1];
    assert(n_categories == labels.size());
}

int MClassifier::classify(const cv::Mat object)
{
    cv::Mat copy = object.clone();
    preprocess(copy);
    infer(copy);

    // get max value
    auto [category, score] = postprocess();

    // return category index if value > threshold else return -1
    if (score > threshold)
        return category;
    else
        return -1;
}

std::pair<int, float> MClassifier::postprocess()
{
    return argmax((float*)runtime->get_buf(output_binding_index), n_categories);
}

const std::vector<std::string>& MClassifier::get_labels()
{
    return labels;
}