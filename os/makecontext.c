
#define _XOPEN_SOURCE
#include <ucontext.h> 
#undef _XOPEN_SOURCE

#include <stdio.h>
#include <errno.h>

void test_func(int num)
{
	printf("test func %d\n", num);
}

int main()
{
	char stack[10 * 1024]; // to small will segment fault

	int a = 3;
	ucontext_t cur_ctx;
	getcontext(&cur_ctx);

	int b = 4;
	// 0. should init new_ctx before makecontext()
	ucontext_t new_ctx = cur_ctx;
	new_ctx.uc_stack.ss_sp = stack;
	new_ctx.uc_stack.ss_size = sizeof(stack);
	new_ctx.uc_stack.ss_flags = 0;

	// 1. will go back to cur_ctx after test_func() return
	new_ctx.uc_link = &cur_ctx;

	cur_ctx.uc_link = &new_ctx; // will not cause dead looping. why?

	// 2. make test_func() as new_ctx`s entry
	// and make new_ctx as the test_func()`s context,
	// so it wil return to it`s uc_link
	makecontext(&new_ctx, (void (*)(void))test_func, 1, 1412); 

	printf("check errno=%d\n", errno);
	// 3. swap current to new_ctx, which will call test_func()
	swapcontext(&cur_ctx, &new_ctx);

	printf("main. a = %d b = %d\n", a, b);

	return 0;
}

