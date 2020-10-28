#pragma once
#include "Element.h"
#include <opencv2/opencv.hpp>

class VideoReader : public Element
{
public:
    virtual void init();
    virtual void process();
    virtual void finalize();

private:
    cv::Mat* out;
    std::string uri;
    cv::VideoCapture cap;
};

class ImageDisplayer : public Element
{
public:
    virtual void init();
    virtual void process();
    virtual void finalize();

private:
    cv::Mat* in;
    std::string title;
};