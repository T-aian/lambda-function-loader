// SPDX-License-Identifier: BSD-3-Clause

#include "server.h"
#include "ipc.h"
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef OUTPUT_TEMPLATE
#define OUTPUT_TEMPLATE "../checker/output/out-XXXXXX"
#endif

static int lib_load(struct lib *lib)
{
	/* Loading our DL */
	lib->handle = dlopen(lib->libname, RTLD_LAZY);
	if (!lib->handle)
		return -1;
	return 0;
}

static int lib_execute(struct lib *lib)
{
	/* We get the address of our function from the DL */
	if (lib->filename[0] != '\0') {
		if (lib->funcname[0] != '\0') {
			lib->p_run = dlsym(lib->handle, lib->funcname);
		} else {
			lib->p_run = dlsym(lib->handle, "run");
		}
	} else {
		if (lib->funcname[0] != '\0') {
			lib->run = dlsym(lib->handle, lib->funcname);
		} else {
			lib->run = dlsym(lib->handle, "run");
		}
	}
	/* If we have no function to run then well too bad. */
	if (!lib->p_run && !lib->run) {
		return -1;
	}
	/* We open our output file. We open before child fork so lib->output file
	gets modified properly by mkstemp(). */
	int out_fd = mkstemp(lib->outputfile);
	int frk = fork();
	if (frk == 0) {
		/* We redirect the output of the child to our output file */
		dup2(out_fd, STDOUT_FILENO);
		close(out_fd);

		/* Running our function */
		if (lib->filename[0] != '\0') {
			lib->p_run(lib->filename);
		} else {

			lib->run();
		}
		exit(0);
	}
	close(out_fd);
	/* We wait for the above child to die. Dying = the function finished
	 * execution, good or not. */
	wait(NULL);

	return 0;
}

static int lib_run(struct lib *lib)
{
	int err = lib_load(lib);
	if (err)
		return err;

	return lib_execute(lib);
}

static int parse_command(const char *buf, char *name, char *func, char *params)
{
	int ret;

	ret = sscanf(buf, "%s %s %s", name, func, params);
	if (ret < 0)
		return -1;

	return ret;
}

void handle_client(int client_fd)
{
	/* The recieved message */
	char recv_buff[BUFSIZE] = {0};
	ssize_t cnt = recv_socket(client_fd, recv_buff, BUFSIZE);
	if (cnt == 0)
		return;
	/* Parsing message with parse_command and populating lib */
	char name[BUFSIZE] = {0};
	char func[BUFSIZE] = {0};
	char params[BUFSIZE] = {0};
	char output[BUFSIZE] = {0};
	parse_command(recv_buff, name, func, params);
	struct lib runnable = (struct lib){.libname = name,
									   .funcname = func,
									   .filename = params,
									   .run = NULL,
									   .p_run = NULL,
									   .outputfile = output};
	strcpy(output, OUTPUT_TEMPLATE);
	/* Handle request from client */
	int ret = lib_run(&runnable);
	/* If there is an error then write it to the output */
	if (ret < 0) {
		int out_fd = mkstemp(runnable.outputfile);

		char send_buff[10 * BUFSIZE];
		memset(send_buff, 0, 10 * BUFSIZE);
		if (strlen(params) != 0)

			snprintf(send_buff, 10 * BUFSIZE,
					 "Error: %s %s %s could not be executed.\n", name, func,
					 params);
		else
			snprintf(send_buff, 10 * BUFSIZE,
					 "Error: %s %s could not be executed.\n", name, func);

		write(out_fd, send_buff, strlen(send_buff));
		close(out_fd);
	}
	/* Send our client the path of the output file */
	char *real = realpath(runnable.outputfile, NULL);
	char send_buff[BUFSIZE];
	memset(send_buff, 0, BUFSIZE);

	send_socket(client_fd, real, strlen(real));
	close(client_fd);
}

int main(void)
{
	/* Auto Flush! */
	setvbuf(stdout, NULL, _IONBF, 0);
	int ret;
	struct lib lib;
	struct sockaddr_un address;
	/* Create our listening socket */
	int sockfd = create_socket();

	int sock_opt = 1;
	int rc =
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sock_opt, sizeof(int));
	if (rc < 0) {
		printf("OPT");
		exit(0);
	}

	memset(&address, 0, sizeof(address));
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, SOCKET_NAME);
	int len = (strlen(address.sun_path) + sizeof(address.sun_family));

	/* Unlinking perhaps previously opened socket */
	unlink(SOCKET_NAME);

	/* Binding and listening */
	rc = bind(sockfd, (struct sockaddr *)&address, len);
	if (rc < 0) {
		printf("BIND");
		exit(0);
	}

	rc = listen(sockfd, MAX_CLIENTS);
	if (rc < 0) {
		printf("LISTEN");
		exit(0);
	}

	/* Main server loop */
	while (1) {
		struct sockaddr_un peer_addr;
		socklen_t lens = sizeof(peer_addr);
		/* Accepting the connection */
		int client_fd = accept(sockfd, (struct sockaddr *)&peer_addr, &lens);
		/* Process the request from another server process */
		int frk = fork();
		if (frk == 0) {
			close(sockfd);
			handle_client(client_fd);
			exit(0);
		}
		/* Continue with the accept loop */
		if (frk > 0) {
			close(client_fd);
		}
	}
	close_socket(sockfd);
	return 0;
}
