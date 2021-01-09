#include "video_stream.h"
#include <cassert>
#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

VideoStream::VideoStream() : mIsValid(false), mFmtCtx(nullptr), mDecCtx(nullptr), mDecOpts(nullptr) {
}

int VideoStream::open(const char * fileName) {
	assert(!mFmtCtx);
	assert(!mDecCtx);

	int errCode = 0;
	AVCodec * decoder = nullptr;
	int streamNo = -1;

	if (avformat_open_input(&mFmtCtx, fileName, nullptr, nullptr) < 0) { errCode = 1; goto err; }
	if (avformat_find_stream_info(mFmtCtx, nullptr) < 0) { errCode = 1; goto err; }
	if ((streamNo = av_find_best_stream(mFmtCtx, AVMediaType::AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0)) < 0) { errCode = 2; goto err; }

	mVideoStream = mFmtCtx->streams[streamNo];

	std::cout << decoder->name << ": " << decoder->long_name << std::endl;

	if (!decoder || decoder->id != AVCodecID::AV_CODEC_ID_H264) { errCode = 3; goto err; }
	if ((mDecCtx = avcodec_alloc_context3(decoder)) == nullptr ) { errCode = 4; goto err; }
	if (avcodec_parameters_to_context(mDecCtx, mVideoStream->codecpar) < 0) { errCode = 5; goto err; }

        //av_dict_set(&mDecOpts, "refcounted_frames", "0", 0);


	if (avcodec_open2(mDecCtx, decoder, &mDecOpts) < 0) { errCode = 6; goto err; }

	mIsValid = true;
	return errCode;
err:
	mIsValid = false;

	mDecOpts = nullptr;
	mVideoStream = nullptr;
	if (mDecCtx) { avcodec_free_context(&mDecCtx); mDecCtx = nullptr; }
	if (mFmtCtx) { avformat_close_input(&mFmtCtx); mFmtCtx = nullptr; }

	return errCode;
}


bool VideoStream::fillNextFrame(Frame & f) const {
	return false;
}

void VideoStream::printStreamInfo() const {
	using namespace std;
	assert(mVideoStream);

	cout << mDecCtx->width <<"x" << mDecCtx->height << " " << mDecCtx->pix_fmt << " " << mVideoStream->avg_frame_rate.num << "/" << mVideoStream->avg_frame_rate.den << " fps" << endl;
//	cout << "Opts: " << endl;
//	for (int i = 0; i < mDecOpts->count; ++i) {
//		cout << "\t\"" << mDecOpts->elems[i].key << "\": \"" << mDecOpts->elems[i].value << "\"" << endl;
//	}
//	cout << "Opts end" << endl;
}
