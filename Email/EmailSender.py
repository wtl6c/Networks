# William Trace LaCour (WTL6C)
# Justin Javier (JTJ5GS)

# Sites Used During Coding Process
# https://en.wikipedia.org/wiki/Simple_Mail_Transfer_Protocol#SMTP_transport_example
# https://www.geeksforgeeks.org/socket-programming-python/
# https://www.greenend.org.uk/rjk/tech/smtpreplies.html


#!/usr/bin/env python3

# Include needed libraries. Do _not_ include any libraries not included with
# Python3 (i.e. do not use `pip`).
import socket


# Establish a TCP connection with the mail server.
s = socket.socket()
s.connect(('128.143.2.9', 25))


# Read greeting from the server
data = s.recv(4096)
response = data.decode('utf-8')

if not response.startswith('220'):
	raise Exception('220 reply not received from server.')

# Send HELO command and get server response.
cmd_HELO = 'HELO Trace\r\n'
print(cmd_HELO)
s.send(cmd_HELO.encode())

response = s.recv(4096).decode('utf-8')
print(response)

if not response.startswith('250'):
	raise Exception('250 reply not received from server.')


# Send MAIL FROM command.
cmd_MAIL_FROM = 'MAIL FROM: wtl6c@virginia.edu\r\n'
print(cmd_MAIL_FROM)
s.send(cmd_MAIL_FROM.encode())

response = s.recv(4096).decode('utf-8')
print(response)

if not response.startswith('250'):
	raise Exception('250 reply not received from server.')


# Send RCPT TO command.
cmd_RCPT_TO = 'RCPT TO: bradjc@virginia.edu\r\n'
print(cmd_RCPT_TO)
s.send(cmd_RCPT_TO.encode())

response = s.recv(4096).decode('utf-8')
print(response)

if not response.startswith('250'):
	raise Exception('250 reply not received from server.')


# Send DATA command.
cmd_DATA = 'DATA\r\n'
print(cmd_DATA)
s.send(cmd_DATA.encode())

response = s.recv(4096).decode('utf-8')
print(response)

if not response.startswith('354'):
	raise Exception('354 Server not responding.')


# Subject Line
cmd_SUBJECT = 'SUBJECT: Project 1 - Email (JTJ5GS & WTL6C)\r\n'
print(cmd_SUBJECT)
s.send(cmd_SUBJECT.encode())


# Send message data.
cmd_MSG = 'This is Project 1 submission for : \n\nJustin Javier (JTJ5GS) \nWilliam Trace LaCour (WTL6C) \n'
print(cmd_MSG)
s.send(cmd_MSG.encode())


# End with line with a single period.
cmd_END = '.\r\n'
print(cmd_END)
s.send(cmd_END.encode())

response = s.recv(4096).decode('utf-8')
print(response)

if not response.startswith('250'):
	raise Exception('250 reply not received from server.')


# Send QUIT command.
cmd_QUIT = 'QUIT\r\n'
print(cmd_QUIT)
s.send(cmd_QUIT.encode())

response = s.recv(4096).decode('utf-8')
print(response)

if not response.startswith('221'):
	raise Exception('221 Service not closed.')


# Close the socket when finished.
print('Closing Socket')
s.close()
