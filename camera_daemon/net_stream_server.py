import threading
import queue
import socket

class NetStreamServer:
	def __init__(self, port):
		self._port = port
		self._active = False
		self._has_client = False;

		self._data_queue = queue.Queue()
		self._process_thread = threading.Thread(target = self._processQueue)

		self._process_thread.start()

	def hasClient(self):
		return self._has_client

	def write(self, s):
		self._data_queue.put(bytearray(s))

	def _processQueue(self):
		print("NetStreamServer - starting")

		try:
			server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			server.bind(("0.0.0.0", self._port))
			server.listen(1)
			print("NetStreamServer - Listening on {0}".format(self._port))
			self._active = True

			while self._active:
				client_conn, client_addr = server.accept()
				print("NetStreamServer - Client connected: {0}".format(str(client_addr)))
				data_conn = client_conn.makefile("wb")
				self._has_client = True
				try:
					while self._has_client and self._active:
						data = self._data_queue.get()
						data_conn.write(data)
						self._data_queue.task_done()
				except IOError as e:
					print("IOError: {0}".format(str(e)))
				finally:
				    self._has_client = False
				    client_conn.close()

		except (socket.error, socket.timeout) as e:
			print(str(e))

		print("NetStreamServer - stopped")

	def stop(self):
		self._active = False;
		self._data_queue.put(bytearray())
		print("NetStreamServer - stopping")
