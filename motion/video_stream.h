#pragma once

/*
 * C++ wrapper of a ffmpeg AVStream to decode H264 video as encoded ny the raspberry pi camera
 * streams Y'UV420p frames
 *
 * based on https://www.ffmpeg.org/doxygen/4.1/demuxing_decoding_8c-example.html
 */

#include <memory>
#include <utility>

struct AVFormatContext;
struct AVCodecContext;
struct AVStream;
struct AVDictionary;
struct AVFrame;

struct YUV420pFrame {
	YUV420pFrame();
	YUV420pFrame(unsigned int idx, unsigned int codedNo, unsigned int width, unsigned int height);
	~YUV420pFrame();

	unsigned int idx, codedNo;
	unsigned int width, height;
	uint8_t * y, *u, *v;
};

using uq_ptr_YUV420pFrame = std::unique_ptr<YUV420pFrame>;
using sh_ptr_YUV420pFrame = std::shared_ptr<YUV420pFrame>;

class VideoStream {
	private:
		bool mIsValid;
		int mStreamIdx;
		int mCrtFrameNo;

		AVFormatContext *mFmtCtx;
		AVStream * mVideoStream;
		AVCodecContext *mDecCtx;
		AVDictionary *mDecOpts;
		AVFrame *mFrame;

		uint8_t *mDstFrame[4];
		int mDstLinesize[4];

	public:

		VideoStream();

		int open(const char * fileName);
		void free();

		// Frame allocator
		void allocFrame(YUV420pFrame & f);

		// frame generator
		uq_ptr_YUV420pFrame getNextFrame();
		bool fillNextFrame(YUV420pFrame & outFrame);

		// info
		void printStreamInfo() const;
		unsigned int getFps() const;
		std::pair<unsigned int, unsigned int> getAvgFps() const;
};

