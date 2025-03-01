import socket

def send_data_to_server(server_address, server_port, data):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        client_socket.connect((server_address, server_port))

        client_socket.sendall(data.encode())

        print(f"Data sent successfully. Data: {data}")

        # Установка таймаута на чтение данных
        client_socket.settimeout(0.2)
        full_message = ""
        while True:
            try:
                response = client_socket.recv(1024)
                if len(response) == 0: 
                    print("Server closed the connection.")
                    break
                full_message += response.decode()
            except socket.timeout:
                break
        
        print(f"Received response from server: {full_message}\n")

    except Exception as e:
        print(f"Error: {e}")

    finally:
        client_socket.close()

if __name__ == "__main__":
    server_address = "127.0.0.1"
    server_port = 5000
    while True:
        data_to_send = input("Enter data to send: ")
        send_data_to_server(server_address, server_port, data_to_send)