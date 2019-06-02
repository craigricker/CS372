#!/bin/python
import argparse
import sys
import socket

GLOBAL_TERM = "**"

def send_message(user_name, conn):
    """
    Using connection created by socket API, prompts user for a message, and then sends it
    through the connection.
    Pre conditions: user_name must be valid text and conn must be active two way socket powered connection
    Post conditions: Message is sent, if user types \quit it will return false, otherwise true
    :param user_name:   Username to be included for prompt and sent message
    :param conn:        Connection to send message on
    :return:            T/F if the user wants to continue the chat conversation
    """
    # Use username to prompt and include in sent message
    to_send = user_name + "> " + input(user_name + "> ")
    # Must encode string to be sent, need simple data type
    conn.sendall(to_send.encode())
    if '\quit' in to_send:
        return False
    else:
        return True


def receive_messsage(conn):
    """
    Through connection created by socket API, receives messages on passed connection (conn) and then displays
    to the user.
    If the message contains "\quit" return False, other wise return true.
    Pre conditions: conn must be active two way socket powered connection
    Post conditions: Message is sent, if user types \quit it will return false, otherwise true
    Used thi
    :param conn:    Connection to receive message from
    :return:        T/F if other user wishes to continue conversation
    """
    data = conn.recv(1024)
    # Decode message so appears as normal text
    data = data.decode('ascii')
    if '\quit' in data:
        return False
    else:
        print(data)
        return True

# https://stackoverflow.com/questions/48812854/python3-sending-files-via-socket
def receive_file(conn, file_name):
    # Open file_name to write to
    writer = open(file_name, 'wb')
    data = conn.recv(1024)
    # Read until end of file
    while data != bytes("".encode()):
        writer.write(data)
        data.recv(1024)





def listen_socket(port):
    """
    Starts listening for connection on specificed port number
    This code is adapted from socket API documentation
    Pre conditions: port must be a valid port number that is not currently in use
    Post condition: initates two way connection to port and returns connection
    :param port:    port to listen for connection
    :return:        created socket
    """
    HOST = ''  # Symbolic name meaning all available interfaces
    my_socket =  socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    my_socket.bind((HOST, port))
    my_socket.listen(1)
    conn, addr = my_socket.accept()
    return conn

def connect_server(host, port):
    to_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    to_server.connect((host, port))
    return to_server


def broadcast_socket(host, port):
    my_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    my_socket.connect((host, port))
    return my_socket

def my_ip():
    """
    # http://stackoverflow.com/questions/24196932/how-can-i-get-the-ip-address-of-eth0-in-python
    :return:
    """
    s = socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    return s.getsockname()[0]


def ftp_me(data_port, single_file=True):
    client_socket = broadcast_socket(client_port)
    # Tell client which port to send file on
    client_socket.send(data_port)
    # Send current IP address as well
    client_socket.send()

    # Either send command to list files, or send specific file
    if single_file:
        # Request single file
        # Check if file exists
        response = receive_messsage(conn)
        if response != "good":
            print(response)
            exit(1)
        else:
            receive_file(conn)

    else:
        # request list of files to chose from
        print(receive_messsage(conn))

    client_socket.close()




if __name__ == "__main__":
    argc = len(sys.argv)
    # Perform input validation on command line prompt
    ports = range(10000, 65536)
    # Need correct command number
    if argc not in range(5, 7):
        print("You must enter 5 or 6 arguments see man for help")
        exit(1)
    elif sys.argv[3] not in ["-g", "-l"]:
        print("You must select -g or -l mode")
        exit(1)
    elif sys.argv[3] == "-g" and argc != 6:
        print("If using -g must specicy a filename")
        exit(1)
    elif int(sys.argv[2]) not in ports or int(sys.argv[-1]) not in ports:
        print("Port must be within range of [10,000 - 65,535]")
        exit(1)
    else:
        host = sys.argv[1]#+".engr.oregonstate.edu"
        host = 'localhost'
        port = int(sys.argv[2])
        to_server = connect_server(host, port)





