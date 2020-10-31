#include "VideoIOElements.h"
#include "Pipeline.h"

void VideoReader::init()
{
    uri = get_property("uri");
    cap.open(uri);
}

void VideoReader::process(std::map<std::string, InPad*>& in_pads, std::map<std::string, OutPad*>& out_pads)
{
    if (!cap.read(image))
    {
        _pipeline->stop();
        return;
    }
    Buffer* buffer = new Buffer();
    buffer->set_buffer("image", image);
    out_pads["out"]->send_buffer(buffer);
}

void VideoReader::finalize()
{
    cap.release();
}

void ImageDisplayer::init()
{
    title = get_property("title");
}

void ImageDisplayer::process(std::map<std::string, InPad*>& in_pads, std::map<std::string, OutPad*>& out_pads)
{
    auto buffer = in_pads["in"]->get_buffer();
    image = std::any_cast<cv::Mat>(buffer->get_buffer("image"));
    cv::imshow(title, image);
    if (cv::waitKey(1) == 27)
        _pipeline->stop();
    delete buffer;
}

void ImageDisplayer::finalize()
{
}