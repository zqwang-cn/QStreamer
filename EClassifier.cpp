#include "EClassifier.h"
#include "EStructures.h"

void EClassifier::init(const std::map<std::string, std::any>& properties, const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    auto config_file = std::any_cast<std::string>(properties.at("config_file"));
    classifier = new MClassifier(config_file);
    name = classifier->get_name();
    labels = classifier->get_labels();
    out_pads.at("out")->send_buffer(in_pads.at("in")->get_buffer());
}

void EClassifier::process(const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer->get_buffer("image"));
    auto objects = std::any_cast<std::list<EObjectInfo>>(buffer->get_buffer("objects"));
    for (auto& obj : objects)
    {
        int category = classifier->classify(image(obj.bbox));
        if (category != -1)
            obj.properties[name] = labels[category];
    }
    buffer->set_buffer("objects", objects);
    out_pads.at("out")->send_buffer(std::move(buffer));
}

void EClassifier::finalize()
{
    delete classifier;
}