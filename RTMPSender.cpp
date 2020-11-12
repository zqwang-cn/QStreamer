#include "RTMPSender.h"
#include <iostream>
extern "C"
{
#include <libavutil/imgutils.h>
}

#define FLV_TIME_BASE AVRational { 1, 1000 }
#define FLV_PTS_INC 40

RTMPSender::RTMPSender(std::string url, int width, int height, int framerate, int stride) : width(width), height(height), stride(stride)
{
	// create output context
	if (avformat_alloc_output_context2(&out_context, NULL, "flv", url.c_str()) < 0)
		report_error("Could not create format context");
	//  open avio
	if (avio_open(&out_context->pb, url.c_str(), AVIO_FLAG_WRITE) < 0)
		report_error("Could not open avio");

	// find encoder
	AVCodec *out_codec = avcodec_find_encoder_by_name("h264_nvenc");
	// AVCodec *out_codec = avcodec_find_encoder_by_name("libx264");
	if (!out_codec)
		report_error("Could not find encoder");
	// create encoder context
	out_codec_context = avcodec_alloc_context3(out_codec);
	if (!out_codec_context)
		report_error("Could not create encoder context");
	// set parameters
	out_codec_context->width = width;
	out_codec_context->height = height;
	out_codec_context->framerate = AVRational{framerate, 1};
	out_codec_context->time_base = FLV_TIME_BASE;
	out_codec_context->codec_tag = 0;
	out_codec_context->pix_fmt = AVPixelFormat::AV_PIX_FMT_YUV420P;
	out_codec_context->bit_rate = 0;
	if (out_context->oformat->flags & AVFMT_GLOBALHEADER)
		out_codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	// open encoder
	if (avcodec_open2(out_codec_context, out_codec, NULL) < 0)
		report_error("Could not open encoder");

	// add new stream to output context
	out_stream = avformat_new_stream(out_context, out_codec);
	if (!out_stream)
		report_error("Could not create stream");
	// copy paramters to new stream
	avcodec_parameters_from_context(out_stream->codecpar, out_codec_context);
	// set metadata
	out_stream->avg_frame_rate = AVRational{framerate, 1};

	// write header
	if (avformat_write_header(out_context, NULL) < 0)
		report_error("Could not write header");

	// init sws context
	sws_context = sws_getContext(width, height, AV_PIX_FMT_BGR24, width, height, AVPixelFormat::AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	if (!sws_context)
		report_error("Could not get sws context");

	// alloc frame structure and set parameters
	frame = av_frame_alloc();
	if (!frame)
		report_error("Could not alloc frame");
	frame->pts = 0;
	frame->format = AVPixelFormat::AV_PIX_FMT_YUV420P;
	frame->width = width;
	frame->height = height;

	// set frame buffer
	if (av_frame_get_buffer(frame, 0) < 0)
		report_error("Could not alloc frame buffer");

	// new packet structure
	packet = av_packet_alloc();
	if (!packet)
		report_error("Could not alloc packet");
}

RTMPSender::~RTMPSender()
{
	free_resources();
}

void RTMPSender::send(uint8_t *data)
{
	// increase timestamp
	frame->pts += FLV_PTS_INC;
	int ret;
	// if not finish, scale and send frame to encoder
	if (data != NULL)
	{
		sws_scale(sws_context, &data, &stride, 0, height, frame->data, frame->linesize);
		ret = avcodec_send_frame(out_codec_context, frame);
	}
	// if finish, flush encoder
	else
		ret = avcodec_send_frame(out_codec_context, NULL);
	if (ret < 0)
	{
		std::cerr << "Error sending a frame for encoding" << std::endl;
		return;
	}
	while (ret >= 0)
	{
		ret = avcodec_receive_packet(out_codec_context, packet);
		// return if no enough data
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0)
		{
			std::cerr << "Error during encoding" << std::endl;
			return;
		}
		// send encoded packet
		if (av_interleaved_write_frame(out_context, packet) < 0)
		{
			std::cerr << "Error writing frame" << std::endl;
		}
		av_packet_unref(packet);
	}
}

void RTMPSender::stop()
{
	// flush encoder
	send(NULL);
	// write trailer
	if (av_write_trailer(out_context) < 0)
	{
		std::cerr << "Could not write trailer" << std::endl;
	}
}

inline void RTMPSender::free_resources()
{
	if (packet)
		av_packet_free(&packet);
	if (frame)
		av_frame_free(&frame);
	if (sws_context)
		sws_freeContext(sws_context);
	if (out_codec_context)
		avcodec_free_context(&out_codec_context);
	if (out_context)
	{
		if (out_context->pb)
			avio_close(out_context->pb);
		avformat_free_context(out_context);
	}
}

inline void RTMPSender::report_error(const char *err)
{
	free_resources();
	std::cerr << err << std::endl;
	throw std::runtime_error(err);
}
