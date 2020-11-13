#include "VideoIOElements.h"
#include "Pipeline.h"

void VideoReader::init(const std::map<std::string, std::any>& properties)
{
    uri = std::any_cast<std::string>(properties.at("uri"));
    cap.open(uri);
}

void VideoReader::process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads)
{
    if (!cap.read(image))
    {
        _pipeline->stop();
        return;
    }
    Buffer* buffer = new Buffer();
    buffer->set_buffer("image", image);
    out_pads.at("out")->send_buffer(buffer);
}

void VideoReader::finalize()
{
    cap.release();
}

void ImageDisplayer::init(const std::map<std::string, std::any>& properties)
{
    title = std::any_cast<std::string>(properties.at("title"));
}

void ImageDisplayer::process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    image = std::any_cast<cv::Mat>(buffer->get_buffer("image"));
    cv::imshow(title, image);
    if (cv::waitKey(1) == 27)
        _pipeline->stop();
    delete buffer;
}

void ImageDisplayer::finalize()
{
}

void RTMPPushStreamElement::init(const std::map<std::string, std::any>& properties)
{
    std::string uri = std::any_cast<std::string>(properties.at("uri"));
    int width = 1024;
    int height = 576;
    int fps = 24;
    int stride = width * 3;
    sender = new RTMPSender(uri, width, height, fps, stride);
}

void RTMPPushStreamElement::process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    image = std::any_cast<cv::Mat>(buffer->get_buffer("image"));
    sender->send(image.data);
    delete buffer;
}

void RTMPPushStreamElement::finalize()
{
    delete sender;
}