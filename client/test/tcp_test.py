import socket

HOST = "127.0.0.1"
PORT = 5000


def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(b"Hello World!")
        data = s.recv(1024)
        s.shutdown(socket.SHUT_WR)

    print(f"RCV: {data!r}")


if __name__ == '__main__':
    main()
