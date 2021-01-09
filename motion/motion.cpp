#include "video_stream.h"

#include <iostream>

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
	} else {
		cout << "err: " << ret << endl;
	}

	return 0;
}

