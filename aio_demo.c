#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <aio.h>
#include <strings.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
//#include <sys/event.h>

char buf[64];

#define IO_SIGNAL SIGUSR1

void signal_handler(int sig, siginfo_t *info, void *ctx)
{
	int ret;
	struct aiocb *my_aiocb = (struct aiocb *)info->si_value.sival_ptr;

	printf("receive signal: %d\n", sig);

	while (aio_error(my_aiocb) == EINPROGRESS)
	{
		write(STDOUT_FILENO, ".", 1); 
		sleep(1);
	}

	printf("aio_error = %d errno = %d\n", aio_error(my_aiocb), errno);

	ret = aio_return(my_aiocb);

	if (ret < 0)
	{
		printf("aio_return = %d\n", ret);
	}

	printf("content: %s\n", (char*)(my_aiocb->aio_buf));
}

void thread_handler(union sigval val)
{
	int ret;
	struct aiocb *my_aiocb = (struct aiocb *)val.sival_ptr;

	printf("thread handler()\n");

	while (aio_error(my_aiocb) == EINPROGRESS)
	{
		write(STDOUT_FILENO, ".", 1); 
		sleep(1);
	}

	printf("aio_error = %d\n", aio_error(my_aiocb));

	ret = aio_return(my_aiocb);

	if (ret < 0)
	{
		printf("aio_return = %d\n", ret);
	}

	printf("content: %s\n", (char*)(my_aiocb->aio_buf));
//	printf("content: %s\n", buf);
}

void test_signal(struct aiocb *my_aiocb)
{
	int ret;

	struct sigaction sa; 
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO; // SA_RESTART, SA_NODEFER
	sa.sa_sigaction = signal_handler;
	sigaction(IO_SIGNAL, &sa, NULL);

	my_aiocb->aio_sigevent.sigev_notify = SIGEV_SIGNAL;
	my_aiocb->aio_sigevent.sigev_signo = IO_SIGNAL;
	my_aiocb->aio_sigevent.sigev_value.sival_ptr = my_aiocb;	

	ret = aio_read(my_aiocb);

	if (ret < 0)
	{
		printf("aio_read() ret = %d\n", ret);
		return;
	}

	printf("aio_error = %d\n", aio_error(my_aiocb));

	printf("waiting for aio...\n");
	pause();
}

void test_thread(struct aiocb *my_aiocb)
{
	int ret;
	const struct aiocb* aio_list[1] = {my_aiocb};

	my_aiocb->aio_sigevent.sigev_notify = SIGEV_THREAD;
	my_aiocb->aio_sigevent.sigev_notify_function = thread_handler;
	my_aiocb->aio_sigevent.sigev_value.sival_ptr = my_aiocb;

	ret = aio_read(my_aiocb);

	if (ret < 0)
	{
		printf("aio_read() ret = %d\n", ret);
		return;
	}

	printf("aio_error = %d\n", aio_error(my_aiocb));

	ret = aio_suspend(aio_list, 1, NULL);
	if (ret < 0)
	{
		printf("aio_suspend() ret = %d\n", ret);
		return;
	}

	printf("waiting for aio...\n");
	pause();

}

void test_normal(struct aiocb *my_aiocb)
{
	int ret;

	ret = aio_read(my_aiocb);
	if (ret < 0) 
	{
		printf("aio_read error.\n");
		return;
	}

	printf("aio_error = %d\n", aio_error(my_aiocb));

	while (aio_error(my_aiocb) == EINPROGRESS)
	{
		write(STDOUT_FILENO, ".", 1); 
		sleep(1);
	}

	printf("content: %s\n", (char*)(my_aiocb->aio_buf));
	printf("aio_error=%d\n", aio_error(my_aiocb));
}

typedef void (*aio_handler_pt)(void *ctx);

struct kqueue_ctx
{
	struct aiocb* my_aiocb;
	aio_handler_pt handler;
};

static void kqueue_aio_handler(void *ctx)
{
	int ret;
	struct kqueue_ctx *back_ctx = (struct kqueue_ctx *)ctx;
	struct aiocb *my_aiocb = (struct aiocb *)back_ctx->my_aiocb;

	fprintf(stderr, "kqueue_aio_handler()\n");

	while (aio_error(my_aiocb) == EINPROGRESS)
	{
		write(STDOUT_FILENO, ".", 1); 
		sleep(1);
	}

	fprintf(stderr, "aio_error = %d errno = %d\n", aio_error(my_aiocb), errno);

	ret = aio_return(my_aiocb);

	if (ret < 0)
	{
		fprintf(stderr, "aio_return = %d errno = %d\n", ret, errno);
	}

	fprintf(stderr, "content: %s\n", (char*)(my_aiocb->aio_buf));
}

void test_kqueue(struct aiocb* my_aiocb)
{
/*
	int kqfd = kqueue();
	if (kqfd == -1)
	{
		fprintf(stderr, "open kqueue() failed. error = %d\n", errno);
		return;
	}

	struct kqueue_ctx ctx;
	ctx.my_aiocb = my_aiocb;
	ctx.handler = kqueue_aio_handler;

	my_aiocb->aio_sigevent.sigev_notify_kqueue = kqfd;
	my_aiocb->aio_sigevent.sigev_notify = SIGEV_KEVENT;
	my_aiocb->aio_sigevent.sigev_value.sigval_ptr = &ctx;

	int ret = aio_read(my_aiocb);

	if (ret < 0)
	{
		fprintf(stderr,"aio_read() ret = %d errno = %d\n", ret, errno);
		perror("PERROR ");
		return;
	}

#define KEVENT_MAX 10

	struct kqueue_ctx *back_ctx;

	struct kevent out[KEVENT_MAX];
	while (1)
	{
		fprintf(stderr, "start to wait.\n"); 
//		ret = kevent(kqfd, NULL, 0, out, 2, NULL or tp);
		ret = kevent(kqfd, NULL, KEVENT_MAX, out, (int)KEVENT_MAX, NULL);

		if (ret < 0)
			return;

		if (ret == 0)
			sleep(1);

		for (int i = 0; i < ret; i++)
		{
			switch (out[i].filter)
			{
			case EVFILT_AIO:
				fprintf(stderr, "aio back\n");
				break;
			default:
				fprintf(stderr, "not possible here\n");
				break;
			}
			back_ctx = (struct kqueue_ctx *) out[i].udata;
			back_ctx->handler(back_ctx);
		}
	}
*/
}

int main()
{
	int fd, ret;
	struct aiocb my_aiocb;

	memset(&my_aiocb, 0, sizeof(struct aiocb));

	my_aiocb.aio_buf = buf;
	my_aiocb.aio_fildes = STDIN_FILENO;
	my_aiocb.aio_nbytes = 64; 
	my_aiocb.aio_offset = 0;

//	test_normal(&my_aiocb);
//	test_signal(&my_aiocb);
	test_thread(&my_aiocb);
//	test_kqueue(&my_aiocb);

	sleep(10);
	return 0;
}
