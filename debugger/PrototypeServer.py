import sys, os
import threading as th
import socket as sk

# This script serves as a prototype server to test the mozart
# debugger client. All data sent to the client are random and fake.

# Sources :
# - https://www.baeldung.com/linux/python-unix-sockets

class PrototypeServer:
    def __init__(self, ip: str, port: int, n_clients: int = 1):
        self.__ip = ip
        self.__port = port
        self.__working = True

        # try:
        #     os.unlink(self.__path)
        # except OSError:
        #     if os.path.exists(self.__path):
        #         raise OSError("Failed to remove existing socket file")
        
        self.__server = sk.socket(sk.AF_INET, sk.SOCK_STREAM)
        self.__server.bind((self.__ip, self.__port))
        self.__server.listen(n_clients)
        
        self.__threads = []

    def stop(self):
        self.__working = False
        self.__server.shutdown(sk.SHUT_RDWR)
        self.__server.close()
        
        for thread in self.__threads:
            thread.join()

    def start(self):
        def handle(self, connection: sk.socket):
            try:
                while True:
                    data = connection.recv(1024)
                    if not data:
                        print(f"Connection {str(connection)} closed")
                        break
                    print(f"Received data: {data.decode()}")
                    connection.sendall("Test".encode())
            except ConnectionError as e:
                print(f"Connection error: {e}")
            finally:
                connection.close()

        server: sk.socket = self.__server
        try:
            while self.__working:
                connection, _ = server.accept()
                print(f"New connection from {str(connection)}")
                thread = th.Thread(target=handle, args=(self, connection))
                thread.start()
                self.__threads.append(thread)
        except KeyboardInterrupt:
            pass
        except Exception as e:
            print(f"Server stopped: {e}")
        finally:
            self.stop()

if __name__ == "__main__":
    pserver = PrototypeServer("127.0.0.1", 51966)

    pserver.start()
    sys.exit(0)