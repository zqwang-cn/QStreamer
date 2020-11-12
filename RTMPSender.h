#pragma once
#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

class RTMPSender
{
public:
	RTMPSender(std::string url, int width, int height, int framerate, int stride);
	~RTMPSender();
	void send(uint8_t* data);
	void stop();
private:
	int width;
	int height;
	int stride;
	AVFormatContext* out_context = NULL;
	AVCodecContext* out_codec_context = NULL;
	AVStream* out_stream = NULL;
	SwsContext* sws_context = NULL;
	AVFrame* frame = NULL;
	AVPacket* packet = NULL;
	void free_resources();
	void report_error(const char* err);
};