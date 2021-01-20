#include "video_stream.h"
#include <cassert>
#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
}

VideoStream::VideoStream() : mIsValid(false), mStreamIdx(-1), mCrtFrameNo(0), mFmtCtx(nullptr), mDecCtx(nullptr), mDecOpts(nullptr) {
}

AVPacket pkt;

int VideoStream::open(const char * fileName) {
	assert(!mFmtCtx);
	assert(!mDecCtx);

	int errCode = 0;
	AVCodec * decoder = nullptr;

	if (avformat_open_input(&mFmtCtx, fileName, nullptr, nullptr) < 0) { errCode = 1; goto err; }
	if (avformat_find_stream_info(mFmtCtx, nullptr) < 0) { errCode = 1; goto err; }
	if ((mStreamIdx = av_find_best_stream(mFmtCtx, AVMediaType::AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0)) < 0) { errCode = 2; goto err; }

	mVideoStream = mFmtCtx->streams[mStreamIdx];

	std::cout << decoder->name << ": " << decoder->long_name << std::endl;

	if (!decoder || decoder->id != AVCodecID::AV_CODEC_ID_H264) { errCode = 3; goto err; }
	if ((mDecCtx = avcodec_alloc_context3(decoder)) == nullptr ) { errCode = 4; goto err; }
	if (avcodec_parameters_to_context(mDecCtx, mVideoStream->codecpar) < 0) { errCode = 5; goto err; }


	if (avcodec_open2(mDecCtx, decoder, &mDecOpts) < 0) { errCode = 6; goto err; }

	if (!(mFrame = av_frame_alloc())) {errCode = 7; goto err; }

	if (av_image_alloc(mDstFrame, mDstLinesize, mDecCtx->width, mDecCtx->height, mDecCtx->pix_fmt, 1) < 0) { errCode = 9; goto err; }

	av_init_packet(&pkt);

	pkt.size = 0;
	pkt.data = nullptr;

	mIsValid = true;
	return errCode;
err:
	free();

	return errCode;
}

void VideoStream::free() {
	if (mIsValid) { av_free(mDstFrame[0]); }

	mIsValid = false;
	mStreamIdx = -1;
	mCrtFrameNo = 0;
	mDecOpts = nullptr;
	mVideoStream = nullptr;

	if (mFrame) { av_frame_free(&mFrame); mFrame = nullptr; }
	if (mDecCtx) { avcodec_free_context(&mDecCtx); mDecCtx = nullptr; }
	if (mFmtCtx) { avformat_close_input(&mFmtCtx); mFmtCtx = nullptr; }
}

YUV420pFrame::YUV420pFrame(unsigned int idx, unsigned int codedNo, unsigned int width, unsigned int height) : 
	idx(idx), codedNo(codedNo), width(width), height(height),
	y(new uint8_t[width * height]),
	u(new uint8_t[width/2 * height/2]),
	v(new uint8_t[width/2 * height/2]) {
}

YUV420pFrame::YUV420pFrame() :
	idx(0), codedNo(0), width(0), height(0),
	y(nullptr),
	u(nullptr),
	v(nullptr) {
}

YUV420pFrame::~YUV420pFrame() {
	delete y;
	delete u;
	delete v;
}

void VideoStream::allocFrame(YUV420pFrame & f) {
	if (f.y) delete f.y;
	if (f.u) delete f.u;
	if (f.v) delete f.v;

	f.idx = 0;
	f.codedNo = 0;
	f.width = mDecCtx->width;
	f.height = mDecCtx->height;

	f.y = new uint8_t[f.width * f.height];
	f.u = new uint8_t[f.width/2 * f.height/2];
	f.v = new uint8_t[f.width/2 * f.height/2];
}

uq_ptr_YUV420pFrame VideoStream::getNextFrame() {
	YUV420pFrame *f = new YUV420pFrame(0, 0, mDecCtx->width, mDecCtx->height);
	
	if (fillNextFrame(*f)) {
		return uq_ptr_YUV420pFrame(f);
	} else {
		return nullptr;
	}
}

bool VideoStream::fillNextFrame(YUV420pFrame& outFrame) {
	int frameReady = 0;
	bool err = false;

	const unsigned yLen = mDecCtx->width * mDecCtx->height;
	const unsigned uvLen = yLen / 4;

	while (!frameReady && !err && av_read_frame(mFmtCtx, &pkt) >= 0) {
		if (pkt.stream_index == mStreamIdx) {
			err = avcodec_decode_video2(mDecCtx, mFrame, &frameReady, &pkt) < 0;
			if (!err && frameReady) {
				if (mFrame->width != mDecCtx->width || mFrame->height != mDecCtx->height || mFrame->format != mDecCtx->pix_fmt) { continue; }

				av_image_copy(mDstFrame, mDstLinesize, (const uint8_t **)mFrame->data, mFrame->linesize, mDecCtx->pix_fmt, mDecCtx->width, mDecCtx->height);

				assert(uvLen * 4 == yLen);

				memcpy(outFrame.y, mDstFrame[0], yLen);
				memcpy(outFrame.u, mDstFrame[1], uvLen);
				memcpy(outFrame.v, mDstFrame[2], uvLen);
				
				outFrame.idx = mCrtFrameNo++;
				outFrame.codedNo = mFrame->coded_picture_number;
				
				return true;
			}
		}
	}

	return false;
}

void VideoStream::printStreamInfo() const {
	using namespace std;
	assert(mVideoStream);

	cout << mDecCtx->width <<"x" << mDecCtx->height << " " << mDecCtx->pix_fmt << " " << mVideoStream->avg_frame_rate.num << "/" << mVideoStream->avg_frame_rate.den << " fps" << endl;
}

std::pair<unsigned int, unsigned int> VideoStream::getAvgFps() const {
	return mIsValid ? 
		std::make_pair(mVideoStream->avg_frame_rate.num, mVideoStream->avg_frame_rate.den) : 
		std::make_pair(0, 0);
}



