#include "VideoIOElements.h"

void VideoReader::init()
{
    uri = get_property("uri");
    cap.open(uri);
}

void VideoReader::process()
{
    cap.read(image);
    Buffer* buffer = new Buffer();
    buffer->set_buffer("image", image);
    find_out_pad("out")->send_buffer(buffer);
}

void VideoReader::finalize()
{
    cap.release();
}

void ImageDisplayer::init()
{
    title = get_property("title");
}

void ImageDisplayer::process()
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