"""
    ProgramName:        ftclient
    Author:             Craig Ricker
    Course Name:        CS372 Intro to Computer Networks
    Last Modified:      June 2nd, 2019
    Description:

    This is the cilent end of the ftclient. It creates a connection using command line
    arguments to an up and running server. It then interacts with the server, and the
    user to either download a specific file, or see the contents of the current directory
    Beej's guide was very helpful, as was my Project 1 for CS372

"""
import sys
from socket import *
import os
import random


def my_ip():
    """
    # https://www.geeksforgeeks.org/python-program-find-ip-address/
    Was helpful in writing this program...I'm not sure how we are
    supposed to read if it is flip1 or flip2 so I am going with iP
    :return: ip address of current system
    """
    hostname = gethostname()
    return gethostbyname(hostname)


def establish_connection(host, port):
    """
    Creates a socket, but an outgoing socket
    :param host: host name to connect to
    :param port: port to connect to
    :return: newly created socket
    """
    client_socket = socket(AF_INET, SOCK_STREAM)
    client_socket.connect((host, port))
    return client_socket


def create_socket(port):
    """
    Function to setup a socket, and wait until a connection comes in
    :param port: port to create a socket to listen to
    :return: newly created socket
    """
    client_port = int(port)
    sockfd = socket(AF_INET, SOCK_STREAM)
    sockfd.bind(('', client_port))
    sockfd.listen(0)
    data_socket, address = sockfd.accept()
    return data_socket


def recv_all(the_socket):
    """
    Repeatedly reads data until connection is broken
    Adopted from # http://code.activestate.com/recipes/408859/
    :param the_socket: socket to read data from
    :return: all data combined into a single str
    """
    total_data=[]
    while True:
        data = the_socket.recv(8192)
        if not data: break
        total_data.append(data.decode())
    return ''.join(total_data)


def recv_all_tofile(the_socket, filename):
    """
    Adopted from # http://code.activestate.com/recipes/408859/
    writes to file, if filename exists already adds random integers
    :param the_socket: socket to read from
    :param filename: file you are requesting
    :return: none
    """
    total_data=[]
    while os.path.isfile(filename):
        filename = filename + str(random.randint(1,101))
    writer = open(filename, 'w')
    while True:
        data = the_socket.recv(8192)
        if not data: break
        writer.write(data.decode())




def talkToServer(client_socket, data_port, command, file_name, host):
    """
    Sends over identifying information to create socket
    Allows server to validate commands, and then executes commands
    :param client_socket: socket to reach server on
    :param data_port: port to have data come back
    :param command: command that is being executed
    :param file_name: if used, file being requested
    :param host: host to connect to
    :return: None
    """
    # Send port number
    client_socket.send(data_port.encode())
    client_socket.recv(1024).decode()
    # Host
    client_socket.send(my_ip().encode())
    client_socket.recv(1024).decode()
    # Command
    client_socket.send(command.encode())
    if (client_socket.recv(1024).decode() == "invalid"):
        print("You have entered an invalid command, exiting")
        exit(1)

    # If requesting file, send file name
    if command == "-g":
        # Send file name
        client_socket.send(file_name.encode())
        response = client_socket.recv(1024).decode()
        if "invalid" in response:
            print("{}:{} says FILE NOT FOUND".format(host, data_port))
            exit(1)
        data_socket = create_socket(data_port)
        print('Receiving "{}" from {}:{}'.format(file_name, host, data_port))
        # Receive and write to file
        recv_all_tofile(data_socket, file_name)
        print("File transfer complete.")
    else:
        print("Receiving directory structure from {}:{}.".format(host, data_port))
        data_socket = create_socket(data_port)
        print(recv_all(data_socket))
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
        host = sys.argv[1]
        # Add eng info if using flip
        port = int(sys.argv[2])
        # Data port is last one, uses trick
        data_port = sys.argv[-1]
        command = sys.argv[3]
        # If -l is used, this is garbage, but never used
        file_name = sys.argv[4]
        client_socket = establish_connection(host, port)
        talkToServer(client_socket, data_port, command, file_name, host)
