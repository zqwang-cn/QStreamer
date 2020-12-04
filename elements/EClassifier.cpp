#include "EClassifier.h"
#include "EStructures.h"

QELEMENT_REGISTER(EClassifier)

void EClassifier::init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto config_file = std::any_cast<std::string>(properties["config_file"]);
    classifier = new MClassifier(config_file);
    name = classifier->get_name();
    labels = classifier->get_labels();
    out_pads["out"]->send_buffer(in_pads["in"]->get_buffer());
}

void EClassifier::process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto buffer = in_pads["in"]->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer["image"]);
    auto objects = std::any_cast<std::list<EObjectInfo>>(buffer["objects"]);
    for (auto& obj : objects)
    {
        int category = classifier->classify(image(obj.bbox));
        if (category != -1)
            obj.properties[name] = labels[category];
    }
    buffer["objects"] = objects;
    out_pads["out"]->send_buffer(std::move(buffer));
}

void EClassifier::finalize()
{
    delete classifier;
}