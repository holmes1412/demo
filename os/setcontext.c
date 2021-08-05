#include <stdio.h>
#include <ucontext.h>

int main()
{
	ucontext_t ctx;
	getcontext(&ctx);
	printf("this will go on forever\n");
	setcontext(&ctx);
	return 0;
}
