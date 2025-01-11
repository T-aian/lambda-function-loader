# Lambda Function Loader
https://github.com/T-aian/lambda-function-loader/tree/main

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

- lib_load: This function loads a shared library specified by lib->libname using dlopen.
- lib_execute: The function first checks if the filename is provided. Then it looks for a function that takes a filename as an argument. It uses dlsym() to get the address of that function. Otherwise it defaults to a function named "run". The function then creates a temporary output file using mkstemp. It also forks a new process using fork(). In the child process: it redirects te standard output to the output file using dup2(), it closes the file descriptor, it runs the retrieved function, it exits the child process. In the parent process: it closes the output file descriptor, it waits for the child process to finish execution using wait().
- lib_run: This function calls lib_load() to load the shared library, and returns the result of lib_execute().
- parse_command: The parse_command function is designed to parse a command string into three components: the library name, the function name, and the parameters.
- handle_client: The function starts by receiving a message from the client using recv_socket. It stores the received message in recv_buff. The function then parses the received message using parse_command. It extracts the library name (name), function name (func), and parameters (params) from the received message. The function populates a lib structure (runnable) with the parsed information. The function calls lib_run to load the shared library and execute the specified function. The function retrieves the real path of the output file using realpath. It sends the path of the output file back to the client using send_socket. Finally, it closes the client socket using close.
- main: The function sets the standard output to be unbuffered using setvbuf. This ensures that all output to stdout is immediately flushed. The function creates a UNIX domain socket using create_socket.
It sets the socket option SO_REUSEADDR to allow the socket to be reused. The function initializes the sockaddr_un structure to zero using memset.
It sets the sun_family field to AF_UNIX to specify a UNIX domain socket.
It copies the socket path (SOCKET_NAME) to the sun_path field. The function unlinks the socket file (SOCKET_NAME) to remove any previous instances of the socket. The function binds the socket to the address specified by address. It sets the socket to listen for incoming connections with a maximum backlog of MAX_CLIENTS. The function enters an infinite loop to accept and handle client connections. It accepts a connection from a client using accept. It forks a new process to handle the client request. The function closes the listening socket using close_socket.
