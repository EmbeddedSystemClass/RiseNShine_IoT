import socket
import sys

host = sys.argv[1]
port = int(sys.argv[2])

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))
while 1:
	inpt = raw_input('Enter text to be sent (or enter "exit" to close): ')
	if inpt == "exit":
		break
	s.send(inpt)
	print s.recv(1024)

#s.shutdown(s.SHUT_RDWR)
s.close()
