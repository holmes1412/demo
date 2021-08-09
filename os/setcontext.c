#include <stdio.h>
#include <ucontext.h>

int main()
{
	ucontext_t ctx;
	int i = 0;
	getcontext(&ctx);
	printf("this will go on. i = %d\n", i);
	if (++i < 2)
		setcontext(&ctx);
	return 0;
}

