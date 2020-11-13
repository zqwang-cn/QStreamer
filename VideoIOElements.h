#pragma once
#include "Element.h"
#include "RTMPSender.h"
#include <opencv2/opencv.hpp>

class VideoReader : public Element
{
public:
    virtual void init(const std::map<std::string, std::any>& properties, const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads);
    virtual void process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads);
    virtual void finalize();

private:
    cv::VideoCapture cap;
};

class ImageDisplayer : public Element
{
public:
    virtual void init(const std::map<std::string, std::any>& properties, const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads);
    virtual void process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads);
    virtual void finalize();

private:
    std::string title;
};

class RTMPPushStreamElement : public Element
{
public:
    virtual void init(const std::map<std::string, std::any>& properties, const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads);
    virtual void process(const std::map<std::string, InPad*>& in_pads, const std::map<std::string, OutPad*>& out_pads);
    virtual void finalize();

private:
    RTMPSender* sender;
};