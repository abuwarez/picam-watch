import threading
import queue
import socket

class NetStreamClient:
	def __init__(self, addr, port):
		self._addr = addr
		self._port = port
		self._active = False

		self._data_queue = queue.Queue()
		self._process_thread = threading.Thread(target = self._processQueue)

		self._process_thread.start()

	def isActive(self):
		return self._active

	def write(self, s):
		self._data_queue.put(bytearray(s))

	def _processQueue(self):
		print("NetStreamClient - starting")

		try:
			client = socket.socket()
			print("NetStreamClient - connecting {0}:{1}".format(self._addr, self._port))
			client.connect((self._addr, int(self._port)))
			print("NetStreamClient - connected")
			data_conn = client.makefile("wb")
			self._active = True
			while self._active:
				data = self._data_queue.get()
				data_conn.write(data)
				self._data_queue.task_done()

			client.close()
		except (socket.error, socket.timeout) as e:
			print(str(e))

		print("NetStreamClient - stopped")

	def stop(self):
		self._active = False;
		self._data_queue.put(bytearray())
		print("NetStreamClient - stopping")
