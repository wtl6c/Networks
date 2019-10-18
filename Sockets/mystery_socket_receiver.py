# William Trace LaCour (WTL6C)
# Justin Javier (JTJ5GS)
# Homework #2 - Sockets Part2

from socket import*

wholeMessage = ""
password = "PASSWORD="
terminator = "."

# Create a socket and notify the server we want to be added to the office
# hour queue.

serverSocket = socket(AF_INET, SOCK_STREAM)

serverSocket.bind(('127.0.0.1', 21212))

serverSocket.listen(1)
connSocket, addr = serverSocket.accept()

while True:

    newMessage = connSocket.recv(2048)
    wholeMessage += newMessage.decode()

    if password in wholeMessage:

        startIndex = wholeMessage.index(password) + 9

        try:

            endIndex = wholeMessage.index(terminator, startIndex)
            extractedPW = wholeMessage[startIndex:endIndex]
            wholeMessage = wholeMessage[(endIndex + 1):]

            print(extractedPW)

        except:

            continue

