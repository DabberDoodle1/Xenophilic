#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

class VideoEncoder {
public:
    VideoEncoder(const char* output_name, int width, int height, int framerate);
    VideoEncoder(const VideoEncoder& other) = delete;
    VideoEncoder(VideoEncoder&& other) = delete;
    VideoEncoder& operator=(const VideoEncoder&& other) = delete;
    VideoEncoder& operator=(VideoEncoder&& other) = delete;
    ~VideoEncoder();

    void add_frame(const uint8_t* pixel_data, const unsigned int frame_index);
private:
    AVCodecContext* codec_context;
    AVFormatContext* format_context;
    AVStream* stream;
    AVFrame* frame;
    AVPacket* packet;
    SwsContext* sws_context;
    const int width;
    const int height;
};
