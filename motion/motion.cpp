#include "video_stream.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#include <cmath>


using namespace std;

class SimpleMotionDetector {
	private:
		const unsigned int mMvThr; //movement threshold, as a percent of the average delta
		uint64_t mAvgDelta; //avg delta pixels between two frames
		uint64_t mMaxDelta;
		uint64_t mFrameCount;

	public:
		SimpleMotionDetector() : mMvThr(150), // (50% above average)
			mAvgDelta(0), mMaxDelta(0), mFrameCount(0) {}

		bool operator()(YUV420pFrame & prev, YUV420pFrame &crt) {
			// Y plane only (b&w)
			const int w = prev.width;
			const int h = prev.height;

			uint64_t delta = 0;
			for (int i = 0; i < w * h; ++i)
				delta += abs(crt.y[i] - prev.y[i]);

			mMaxDelta = std::max(mMaxDelta, delta);

			mAvgDelta = (mAvgDelta * mFrameCount + delta)/(mFrameCount + 1);
			++mFrameCount;

			if (delta > mAvgDelta * mMvThr / 100)
				std::cout << delta << "/" << mAvgDelta << std::endl;

			return delta > mAvgDelta * mMvThr / 100;
		}
};

int main(int argc, char **argv) {
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " <h264 video file> " << endl;
		return 1;
	}

	const unsigned int preMotionSecs = 2;
	const unsigned int postMotionSecs= 8;

	VideoStream vs;
	
	int ret = 0;
	if ((ret = vs.open(argv[1]) == 0)) {
		vs.printStreamInfo();

		auto avgFps = vs.getAvgFps();

		YUV420pFrame a, b, *prev = &a, *crt = &b; // used to compare two consecutive frames
		vs.allocFrame(a);
		vs.allocFrame(b);

		SimpleMotionDetector motionDetected;

		if (vs.fillNextFrame(*prev)) {
			unsigned int remMotionFrames = 0;
			unsigned int startTime = 0;
			while (vs.fillNextFrame(*crt)) {
				if (motionDetected(*prev, *crt)) {
					if (!remMotionFrames) {
						const unsigned int crtTime = crt->codedNo * avgFps.second / avgFps.first;
						startTime = std::max(0U, crtTime - preMotionSecs);
						std::cout << crt->idx << " " << crt->codedNo << " " << crtTime << " " << std::endl;
					}
					remMotionFrames = postMotionSecs * avgFps.first / avgFps.second;
				}
			 	else {
					if (remMotionFrames) {
						if (--remMotionFrames == 0) {
							const unsigned int crtTime = crt->codedNo * avgFps.second / avgFps.first;
							std::cout << "-ss " << startTime << " -t " << crtTime - startTime << std::endl;
							startTime = 0;
						}
					}
				}
				std::swap(prev, crt);
			}
		}
/*
		std::ofstream x("v", std::ofstream::binary);
		while (uq_ptr_YUV420pFrame frame = vs.getNextFrame()) {
			x.write(reinterpret_cast<char *>(frame->y), frame->width * frame->height);
			x.write(reinterpret_cast<char *>(frame->u), frame->width / 2 * frame->height / 2);
			x.write(reinterpret_cast<char *>(frame->v), frame->width / 2 * frame->height / 2);
			printf("%u %u\n", frame->idx, frame->codedNo);
		}

	x.close();
*/
	} else {
		cout << "err: " << ret << endl;
	}

	return 0;
}

