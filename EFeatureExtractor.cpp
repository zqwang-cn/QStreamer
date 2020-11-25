#include "EFeatureExtractor.h"
#include "EStructures.h"

QELEMENT_REGISTER(EFeatureExtractor)

void EFeatureExtractor::init(const std::map<std::string, std::any>& properties, const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    auto config_file = std::any_cast<std::string>(properties.at("config_file"));
    extractor = new MFeatureExtractor<float>(config_file);
    out_pads.at("out")->send_buffer(in_pads.at("in")->get_buffer());
}

void EFeatureExtractor::process(const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer.get_buffer("image"));
    auto objects = std::any_cast<std::list<EObjectInfo>>(buffer.get_buffer("objects"));
    for (auto& obj : objects)
        obj.feature = extractor->extract(image(obj.bbox));
    buffer.set_buffer("objects", objects);
    out_pads.at("out")->send_buffer(std::move(buffer));
}

void EFeatureExtractor::finalize()
{
    delete extractor;
}