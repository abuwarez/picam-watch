#include "video_stream.h"

#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char **argv) {
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " <h264 video file> " << endl;
		return 1;
	}

	VideoStream vs;
	int ret = 0;
	if ((ret = vs.open(argv[1]) == 0)) {
		vs.printStreamInfo();
		std::ofstream x("v", std::ofstream::binary);
		while (uq_ptr_YUV420pFrame frame = vs.nextFrame()) {
			x.write(reinterpret_cast<char *>(frame->y), frame->width * frame->height);
			x.write(reinterpret_cast<char *>(frame->u), frame->width / 2 * frame->height / 2);
			x.write(reinterpret_cast<char *>(frame->v), frame->width / 2 * frame->height / 2);
			printf("%u %u\n", frame->idx, frame->codedNo);
		}
		x.close();
	} else {
		cout << "err: " << ret << endl;
	}

	return 0;
}

