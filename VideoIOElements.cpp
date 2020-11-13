#include "VideoIOElements.h"
#include "Pipeline.h"

void VideoReader::init(const std::map<std::string, std::any>& properties, const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads)
{
    auto uri = std::any_cast<std::string>(properties.at("uri"));
    cap.open(uri);
    std::map<std::string, int> input_meta;
    int width = int(cap.get(CV_CAP_PROP_FRAME_WIDTH));
    input_meta["width"] = width;
    input_meta["height"] = int(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    input_meta["fps"] = int(cap.get(CV_CAP_PROP_FPS));
    input_meta["stride"] = width * 3;
    auto buffer = new Buffer();
    buffer->set_buffer("input_meta", input_meta);
    out_pads.at("out")->send_buffer(buffer);
}

void VideoReader::process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads)
{
    cv::Mat image;
    if (!cap.read(image))
    {
        _pipeline->stop();
        return;
    }
    auto buffer = new Buffer();
    buffer->set_buffer("image", image);
    out_pads.at("out")->send_buffer(buffer);
}

void VideoReader::finalize()
{
    cap.release();
}

void ImageDisplayer::init(const std::map<std::string, std::any>& properties, const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads)
{
    title = std::any_cast<std::string>(properties.at("title"));
    auto buffer = in_pads.at("in")->get_buffer();
    delete buffer;
}

void ImageDisplayer::process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer->get_buffer("image"));
    cv::imshow(title, image);
    if (cv::waitKey(1) == 27)
        _pipeline->stop();
    delete buffer;
}

void ImageDisplayer::finalize()
{
}

void RTMPPushStreamElement::init(const std::map<std::string, std::any>& properties, const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads)
{
    auto uri = std::any_cast<std::string>(properties.at("uri"));
    auto buffer = in_pads.at("in")->get_buffer();
    auto input_meta = std::any_cast<std::map<std::string, int>>(buffer->get_buffer("input_meta"));
    int width = input_meta.at("width");
    int height = input_meta.at("height");
    int fps = input_meta.at("fps");
    int stride = input_meta.at("stride");
    delete buffer;
    sender = new RTMPSender(uri, width, height, fps, stride);
}

void RTMPPushStreamElement::process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    auto image = std::any_cast<cv::Mat>(buffer->get_buffer("image"));
    sender->send(image.data);
    delete buffer;
}

void RTMPPushStreamElement::finalize()
{
    delete sender;
}