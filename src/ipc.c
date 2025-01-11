// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include "ipc.h"


int create_socket(void)
{
	/* TODO: Implement create_socket(). */
	int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockfd < 0) {
		printf("SOCKET problem");
		exit(0);
	}
	return sockfd;
}

int connect_socket(int fd)
{
	struct sockaddr_un address;

	memset(&address, 0, sizeof(address));
	address.sun_family = AF_UNIX;
	strncpy(address.sun_path, SOCKET_NAME, sizeof(SOCKET_NAME));
	return connect(fd, (struct sockaddr *) &address, sizeof(address));
}

ssize_t send_socket(int fd, const char *buf, size_t len)
{
	int total_bytes_sent = 0;
	int bytes_sent = 1;
	while(total_bytes_sent < len && bytes_sent != -1) {
		bytes_sent = send(fd, buf + total_bytes_sent, len - total_bytes_sent, 0);
		if(bytes_sent > 0) total_bytes_sent += bytes_sent;
	}
	return total_bytes_sent;
}

ssize_t recv_socket(int fd, char *buf, size_t len)
{
	// printf("prerecving\n");
	ssize_t send_bytes = 1;
	// ssize_t total_send_bytes = 0;
	// while(total_send_bytes < len && send_bytes != -1) {
		// printf("recving\n");
		// send_bytes = recv(fd, buf+total_send_bytes, len - total_send_bytes, 0);
		// printf("[DEBUG] recv: %d", send_bytes);
		// if(send_bytes > 0) total_send_bytes += send_bytes;
	// }
	// return total_send_bytes;
	return recv(fd, buf, len, 0);

}

void close_socket(int fd)
{
	close(fd);
}
