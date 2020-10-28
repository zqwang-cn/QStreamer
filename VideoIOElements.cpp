#include "VideoIOElements.h"

void VideoReader::init()
{
    out = (cv::Mat*)find_out_pad("out")->get_buffer().get();
    uri = get_property("uri");
    cap.open(uri);
}

void VideoReader::process()
{
    cap.read(*out);
}

void VideoReader::finalize()
{
    cap.release();
}

void ImageDisplayer::init()
{
    in = (cv::Mat*)find_in_pad("in")->get_buffer().get();
    title = get_property("title");
}

void ImageDisplayer::process()
{
    cv::imshow(title, *in);
    cv::waitKey(1);
}

void ImageDisplayer::finalize()
{
}