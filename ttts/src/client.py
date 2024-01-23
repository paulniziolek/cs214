import socket
import threading

def receive_messages(client_socket):
    while True:
        try:
            message = client_socket.recv(1024).decode()
            if message:
                print(message)
                # TODO...
            else:
                break
        except Exception as e:
            print(f"Error receiving data: {e}")
            break

def client_program():
    host = "128.6.13.3"  # Server's IP address
    #host = "127.0.0.1"
    port = 3490          # Server's port
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print("attempting connection...")
    client_socket.connect((host, port))
    print("connected succesfully...")

    threading.Thread(target=receive_messages, args=(client_socket,), daemon=True).start()

    while True:
        message = input(" -> ")
        if message.lower() == 'quit':
            break
        # TODO...


        client_socket.send(message.encode())

    client_socket.close()

if __name__ == '__main__':
    client_program()
