from picamera import PiCamera
from time import sleep
from datetime import datetime
from os import path, listdir

from cmd_udp_server import CmdUdpServer
from net_stream_client import NetStreamClient
from net_stream_server import NetStreamServer
from threading import Lock

class MultiOutput(object):

	def _init_new_file(self, is_first = False):
		file_name = str(self._next_file_no).zfill(6)
		if is_first:
			file_name += "x"
		self._size = 0

		try:
			self._fd = open(path.join(self._rec_location, file_name), "wb")
			print("{0} opened".format(file_name))
			self._next_file_no += 1
		except OSError as err:
			print("error opening {0}: {1}".format(file_name, str(err)))

	def __init__(self, rec_location, rec_size = 10000000):
		self._rec_location = rec_location
		self._rec_size = rec_size
		
                # TODO: net_stream_client is obsolete and needs to be removed
		self._net_stream_client = None
		self._net_strean_client_mtx = Lock()

                # TODO: from configuration
		self._net_stream_server = NetStreamServer(12345)

		self._fd = None
		self._next_file_no = self._find_next_file_no(self._rec_location)

		print("Rec Location: {0}".format(self._rec_location))
		print("Res size: {0}".format(self._rec_size))
		print("Next File #: {0}".format(self._next_file_no))

		self._init_new_file(True)

	def write(self, s):
		self._fd.write(s)
		self._fd.flush()
		self._size += len(s)
		if (self._size >= self._rec_size):
			self._fd.close()
			self._init_new_file()

		if self._net_stream_server.hasClient():
			self._net_stream_server.write(s)

		try:
			self._net_strean_client_mtx.acquire()
			if self._net_stream_client and self._net_stream_client.isActive():
				self._net_stream_client.write(s)
		finally:
			self._net_strean_client_mtx.release()

	def flush(self):
		self._fd.flush()
		if (self._size >= self._rec_size):
			self._fd.close()
			self._init_new_file()

	def _find_next_file_no(self, location):
		next_file_no = 0
		for f in listdir(location):
			try:
				crt_file_no = int(f) if not f.endswith('x') else int(f[:-1])
				if crt_file_no >= next_file_no:
					next_file_no = crt_file_no + 1
			except ValueError:
				print("Invalid file name {0}".format(f))
		return next_file_no

	def start_net_stream(self, addr, port):
		print("Net stream to {0}:{1}".format(addr, port))
		try:
			self._net_strean_client_mtx.acquire()
			if self._net_stream_client:
				self._net_stream_client.stop()
			self._net_stream_client = NetStreamClient(addr, port)
		except RuntimeError as e:
			print(str(e))
		finally:
			self._net_strean_client_mtx.release()


	def stop_net_stream(self):
		print("Stopping net stream")
		try:
			self._net_strean_client_mtx.acquire()
			if self._net_stream_client:
				self._net_stream_client.stop()
				self._net_stream_client = None
		except RuntimeError as e:
			print(str(e))
		finally:
			self._net_strean_client_mtx.release()

def start_camera_daemon():

        # TODO: read configuration from file or command line

	camera = PiCamera()
	multi_output = MultiOutput(rec_location = "/data/recs", rec_size = 100000000)
		
	# command server on UDP
	cmd_upd_server = CmdUdpServer(11111, 
		net_stream_start = lambda addr, port: multi_output.start_net_stream(addr, port),
		net_stream_stop = lambda: multi_output.stop_net_stream())

	camera.start_preview()
	
	sleep(2)
	
        #TODO: camera settings via UDP command server
	camera.start_recording(multi_output, format='h264')
	
	while True:
		camera.wait_recording(10)
	
	camera.stop_recording()
	camera.stop_preview()


if __name__ == '__main__':
	start_camera_daemon()
