#include "EFeatureExtractor.h"
#include "EStructures.h"

QELEMENT_REGISTER(EFeatureExtractor)

void EFeatureExtractor::init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto config_file = std::any_cast<std::string>(properties["config_file"]);
    extractor = new MFeatureExtractor<float>(config_file);
    out_pads["out"]->send_buffer(in_pads["in"]->get_buffer());
}

void EFeatureExtractor::process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto buffer = in_pads["in"]->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer["image"]);
    auto objects = std::any_cast<std::list<EObjectInfo>>(buffer["objects"]);
    for (auto& obj : objects)
        obj.feature = extractor->extract(image(obj.bbox));
    buffer["objects"] = objects;
    out_pads["out"]->send_buffer(std::move(buffer));
}

void EFeatureExtractor::finalize()
{
    delete extractor;
}