#include "VideoIOElements.h"

void VideoReader::init()
{
    uri = get_property("uri");
    cap.open(uri);
}

void VideoReader::process(std::map<std::string, Pad*>& in_pads, std::map<std::string, Pad*>& out_pads)
{
    cap.read(image);
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

void ImageDisplayer::process(std::map<std::string, Pad*>& in_pads, std::map<std::string, Pad*>& out_pads)
{
    auto buffer = get_buffer("in");
    image = std::any_cast<cv::Mat>(buffer->get_buffer("image"));
    cv::imshow(title, image);
    cv::waitKey(1);
    delete buffer;
}

void ImageDisplayer::finalize()
{
}