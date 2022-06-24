#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>

void sig_handler(int signo)
{
}

int main(int argc, const char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s EXEC_PROCESS PORT\nBoostrap for workflow server to restart gracefully.\n",
				argv[0]);
		exit(1);
	}

	unsigned short port = atoi(argv[2]);
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sin;

	memset(&sin, 0, sizeof sin);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listen_fd, (struct sockaddr *)&sin, sizeof sin) < 0)
	{
		close(listen_fd);
		perror("bind failed");
		exit(1);
	}

	char cmd[100] = {0};
	pid_t pid;
	int pipe_fd[2];
	char msg_buf[1024];

	char listen_fd_str[10] = { 0 };
	sprintf(listen_fd_str, "%d", listen_fd);

	int cnt = 0;
	while (cnt++ < 10 && (strlen(cmd) == 0 || strncmp(cmd, "restart", 7) == 0))
	{
		memset(cmd, 0, sizeof cmd);
		if (pipe(pipe_fd) == -1)
		{
			perror("open pipe error");
			exit(1);
		}

		char write_fd_str[10] = { 0 };
		sprintf(write_fd_str, "%d", pipe_fd[1]);
		pid = fork();

		if (pid < 0)
		{
			perror("fork error");
			break;
		}
		else if (pid == 0)
		{
			close(pipe_fd[0]);
			execlp(argv[1], argv[1], listen_fd_str, write_fd_str, NULL);
		}
		else
		{
			signal(SIGINT, sig_handler);
			signal(SIGTERM, sig_handler);

			close(pipe_fd[1]);
			signal(SIGCHLD, SIG_IGN);
			fprintf(stderr, "parent wait for cmd: ");
			fgets(cmd, 100, stdin);

			kill(pid, SIGUSR1);
			fprintf(stderr, "parent SIGUSR1 to pid-%d and wait for read\n", pid);
			int len = read(pipe_fd[0], msg_buf, 6);
			fprintf(stderr, "parent read msg:[%*s]\n", len, msg_buf);
			close(pipe_fd[0]);
		}
	}

	close(listen_fd);
    return 0;
}

