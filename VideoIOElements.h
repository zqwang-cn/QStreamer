#pragma once
#include "Element.h"
#include "RTMPSender.h"
#include <opencv2/opencv.hpp>

class VideoReader : public Element
{
public:
    virtual void init(const std::map<std::string, std::string>& properties);
    virtual void process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads);
    virtual void finalize();

private:
    cv::Mat image;
    std::string uri;
    cv::VideoCapture cap;
};

class ImageDisplayer : public Element
{
public:
    virtual void init(const std::map<std::string, std::string>& properties);
    virtual void process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads);
    virtual void finalize();

private:
    cv::Mat image;
    std::string title;
};

class RTMPPushStreamElement : public Element
{
public:
    virtual void init(const std::map<std::string, std::string>& properties);
    virtual void process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads);
    virtual void finalize();

private:
    cv::Mat image;
    RTMPSender* sender;
};