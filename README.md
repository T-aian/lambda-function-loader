# Lambda Function Loader
(Github link here)

## Team

- Ana-Maria Serbanoiu (ana.serbanoiu@stud.acs.upb.ro)
- Andrei-Traian Dobrescu (andrei.dobrescu2402@stud.acs.upb.ro)



## IPC.C

- create_socket: We created a new socket with the socket() function. We used the AF_UNIX constant to specify the address family of UNIX domain sockets. UNIX domain sockets are used for inter-process communication on the same host, providing a way for processes to communicate with each other using file system pathnames as addresses.
- connect_socket: It is used for biding a socket to an address, we use the sockaddr_un structure to specify the address. The sun_family field of this structure is set to AF_UNIX to show its a UNIX domain socket. The memset function initializes the address structure to zero. The strncpy() copies the path of the socket file into the sun_path field of the address structure. SOCKET_NAME is a constant that specifies the path to the socket file. The connect() line attempts to connect the socket referred to by the file descriptor fd to the address specified by address. The connect function establishes a connection to the server socket.
- send_socket: The send_socket function is used to send data over a socket. This function ensures that all the data is sent, even if the send system call sends only a part of the data in one call. We send data in a loop until all data is sent. The send system call attempts to send the remaining data. The buf + total_bytes_sent pointer ensures that the correct part of the buffer is sent. The len - total_bytes_sent value specifies the number of bytes left to send. The send function returns the number of bytes actually sent, which is stored in bytes_sent
- recv_socket: The send function returns the number of bytes actually sent, which is stored in bytes_sent 
- close_socket: The close_socket function is used to close a socket file descriptor. This is important to release the resources associated with the socket and to ensure that the connection is properly terminated.

## SERVER.C