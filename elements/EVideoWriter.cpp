#include "EVideoWriter.h"

QELEMENT_REGISTER(EVideoWriter)

void EVideoWriter::init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto uri = std::any_cast<std::string>(properties["uri"]);
    auto fourcc = std::any_cast<std::string>(properties["fourcc"]);
    auto buffer = in_pads["in"]->get_buffer();
    auto input_meta = std::any_cast<std::map<std::string, int>>(buffer["input_meta"]);
    int width = input_meta.at("width");
    int height = input_meta.at("height");
    int fps = input_meta.at("fps");
    writer.open(uri, CV_FOURCC(fourcc[0], fourcc[1], fourcc[2], fourcc[3]), fps, cv::Size(width, height));
    printf("start\n");
}

void EVideoWriter::process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto buffer = in_pads["in"]->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer["image"]);
    writer.write(image);
}

void EVideoWriter::finalize()
{
    writer.release();
    printf("done\n");
}