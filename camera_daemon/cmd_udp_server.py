import socket
import threading 

class CmdUdpServer:
	def __init__(self, port, net_stream_start, net_stream_stop):
		self._port = port
		self._commands = {}
		self._commands["start_net_stream"] = net_stream_start
		self._commands["stop_net_stream"] = net_stream_stop

		self._thread = threading.Thread(target = self.listen)
		self._thread.start()

	def listen(self):
		cmd_udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		cmd_udp_sock.bind(("0.0.0.0", self._port))
		
		print("CmdUdpServer listening on port {0}".format(self._port))

		while True:
			data, addr = cmd_udp_sock.recvfrom(128)
			print("received \"{0}\" from {1}".format(data, addr))

			msg_toks = data.decode("utf-8")[:-1].split(" ")
			print(msg_toks)
			try:
				cmd = self._commands[msg_toks[0]]
				try:
					cmd(*tuple(msg_toks[1:]))
				except:
					print("Command call failed!")
			except KeyError:
				print("Unknown command")

