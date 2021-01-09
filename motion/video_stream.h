#pragma once

/*
 * C++ wrapper of a ffmpeg AVStream to decode H264 video as encoded ny the raspberry pi camera
 * streams Y'UV420p frames
 *
 * based on https://www.ffmpeg.org/doxygen/4.1/demuxing_decoding_8c-example.html
 */

struct AVFormatContext;
struct AVCodecContext;
struct AVStream;
struct AVDictionary;

struct Frame {};



class VideoStream {
	private:
		bool mIsValid;

		AVFormatContext *mFmtCtx;
		AVStream * mVideoStream;
		AVCodecContext *mDecCtx;
		AVDictionary *mDecOpts;


	public:
		VideoStream();

		int open(const char * fileName);
		bool fillNextFrame(Frame & f) const;

		void printStreamInfo() const;
};

