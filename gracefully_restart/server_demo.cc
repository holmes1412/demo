#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "workflow/WFFacilities.h"
#include "workflow/WFHttpServer.h"

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
	wait_group.done();
}

int main(int argc, const char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s listen_fd pipe_fd\n", argv[0]);
		return -1;
	}

	int listen_fd = atoi(argv[1]);
	int pipe_fd = atoi(argv[2]);

	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGUSR1, sig_handler);

	WFHttpServer server([](WFHttpTask *task) {
     	task->get_resp()->append_output_body("<html>Hello World!</html>");
	});

	if (server.serve(listen_fd) == 0)
	{
		wait_group.wait();
		server.shutdown();
		write(pipe_fd, "success", strlen("success"));
		close(pipe_fd);
		server.wait_finish();
		return 0;
	}

	write(pipe_fd, "failed", strlen("failed"));
	close(pipe_fd);
    return -1;
}

