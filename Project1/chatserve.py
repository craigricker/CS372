import socket
import sys

USER_NAME = "ServerMan"

# def start_up(PORT):




def send_message(user_name, conn):
    to_send = user_name + "> " + input(user_name + "> ")
    conn.sendall(to_send.encode())
    if '\quit' in to_send:
        return False
    else:
        return True


def receive_messsage(conn):
    data = conn.recv(1024)
    data = data.decode('ascii')
    if '\quit' in data:
        return False
    else:
        print(data)
        return True



if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Script format: python chatserve.py PORTNUMBER")
        exit(-1)
    else:
        port = int(sys.argv[1])
    # Start up with port passed in through command line
    user_name = input("Please select a user name 10 digits or less")
    HOST = ''  # Symbolic name meaning all available interfaces
    my_socket =  socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    my_socket.bind((HOST, port))
    my_socket.listen(1)
    conn, addr = my_socket.accept()

    while True:
        # print("Waiting for new message")
        # receive_messsage(conn)
        while (receive_messsage(conn)):
            if not send_message(user_name, conn):
                break
        conn, addr = my_socket.accept()


