# William Trace LaCour (WTL6C)
# Justin Javier (JTJ5GS)
# Homework #2 - Sockets Part1

from socket import*
import sys


# Get command line arguments
host = sys.argv[1]
port = int(sys.argv[2])
name = sys.argv[3]


# Create a socket and notify the server we want to be added to the office
# hour queue.

clientSocket = socket(AF_INET, SOCK_DGRAM)
clientSocket.sendto(str.encode(name), (host, port))


# Wait for head of queue message from the server.

modMsg, svrAddr = clientSocket.recvfrom(2048)
print(modMsg.upper().decode(), "\n")

msg, Addr = clientSocket.recvfrom(2048)
print(msg.upper().decode(), "\n")
print("Press any key when done to dequeue")


# Wait for user to signal that we are done (via stdin) and notify the server.

msg = input()
clientSocket.sendto(str.encode(msg), (host, port))


# Closing Socket

print("closing socket")
clientSocket.close()
