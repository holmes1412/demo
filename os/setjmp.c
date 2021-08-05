#include <stdio.h>
#include <setjmp.h>

static jmp_buf buffer;

void print1()
{
	printf("will print second\n");
	longjmp(buffer, 0); // val will influent what?
	printf("will never print\n");
}

void print2()
{
	printf("will print first\n");
	print1();
	printf("will never print\n");
}

int main()
{
	if (setjmp(buffer) == 0)
	{
		print2();
	}
	else
	{
		printf("will print after longjmp() is call, and if (setjmp()...) is recover again and get ret==1\n");
	}
	return 0;
}
