# William Trace LaCour (WTL6C)
# Justin Javier (JTJ5GS)
# Homework #2 - Sockets Part1

from socket import*
import sys


# Get command line argument
port = int(sys.argv[1])

# Setup socket
serverSocket = socket(AF_INET, SOCK_DGRAM)
serverSocket.bind(('', port))

print("\nThe server is ready to receive on port", port)

# Array to keep keep track of queued clients
waiting_clients = list()

# Continuously receive packets\
while True:

    # Handle the message correctly. Check if this client needs to be added or
    # removed from the queue, and if a new client needs to be notified.
    message, address = serverSocket.recvfrom(2048)

    if address in waiting_clients:
        waiting_clients.remove(address)

    else:
        waiting_clients.append(address)
        Msg = str.encode("You have been added to the queue")
        serverSocket.sendto(Msg, address)

    if len(waiting_clients) > 0:
        address = waiting_clients[0]
        modMsg = str.encode("You are now at the top of the queue")
        serverSocket.sendto(modMsg, address)
