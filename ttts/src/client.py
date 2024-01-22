import socket
import selectors
import sys

MAXBUF = 1024
board = [["." for _ in range(3)] for _ in range(3)]
sel = selectors.DefaultSelector()

def print_board():
    for row in board:
        print(" ".join(row))
    print()

def process_response(tokenized_data, sock):
    resp = tokenized_data[0]
    if resp == "WAIT":
        print("Server connected... waiting for opponent.")
    elif resp == "BEGN":
        print(f"Match found against {tokenized_data[2]}. You are {tokenized_data[1]}.")
        print("X goes first.")
        print_board()
    elif resp == "MOVD":
        x, y, player = int(tokenized_data[1]), int(tokenized_data[2]), tokenized_data[3]
        board[x][y] = player
        print(f"Player {player} played ({x}, {y})")
        print_board()
    elif resp == "INVL":
        print(f"INVALID Server Response: {tokenized_data[1]}")
    elif resp == "OVER":
        print(f"Game has ENDED in favor of {tokenized_data[1]}")
        print_board()
    else:
        print("Unrecognized Server Response...")
        print(tokenized_data)

def handle_user_input(sock):
    print("1) MOVE\n2) RESIGN\n3) REFRESH\n")
    resp = input(" -> ")
    if resp == "1":
        print_board()
        print("Enter move in X,Y format (0-indexed).")
        move = input(" -> ")
        move = move.split(",")
        msg = f"MOVE|{move[0]}|{move[1]}|"
        sock.send(msg.encode())
    elif resp == "2":
        print("Resigning...")
        sock.send("RSGN|".encode())
    elif resp == "3":
        print("Refreshing...")
        print_board()

def read(sock):
    try:
        data = sock.recv(MAXBUF).decode()
        if data:
            tokenized_data = data.split("|")
            process_response(tokenized_data, sock)
        else:
            print("Disconnected from server.")
            sel.unregister(sock)
            sock.close()
    except Exception as e:
        print(f"Error reading from server: {e}")
        sel.unregister(sock)
        sock.close()

def client_program():
    host = socket.gethostname()
    port = 3490
    client_socket = socket.socket()
    client_socket.setblocking(False)
    name = input("Insert name: ")

    try:
        client_socket.connect_ex((host, port))
        sel.register(client_socket, selectors.EVENT_READ, read)
        print("Connection successfully established!")
    except Exception as e:
        print(f"Connection refused: {e}")
        return

    msg = f"PLAY|{name}|"
    client_socket.send(msg.encode())

    while True:
        events = sel.select(timeout=0)
        for key, mask in events:
            callback = key.data
            callback(key.fileobj)

        # Check if we're still connected
        if not client_socket.fileno():
            break

        handle_user_input(client_socket)

if __name__ == '__main__':
    client_program()
