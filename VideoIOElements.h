#pragma once
#include "Element.h"
#include <opencv2/opencv.hpp>

class VideoReader : public Element
{
public:
    virtual void init();
    virtual void process(std::map<std::string, Pad*>& in_pads, std::map<std::string, Pad*>& out_pads);
    virtual void finalize();

private:
    cv::Mat image;
    std::string uri;
    cv::VideoCapture cap;
};

class ImageDisplayer : public Element
{
public:
    virtual void init();
    virtual void process(std::map<std::string, Pad*>& in_pads, std::map<std::string, Pad*>& out_pads);
    virtual void finalize();

private:
    cv::Mat image;
    std::string title;
};
