/*
 * gcc listpack.c test_list.c -o test_list -I./ --std=c99
*/

#include <stdio.h>
#include <string.h>
#include "listpack.h"

unsigned char *buf = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

void test1()
{
	printf("\n== test1 ==\n");
	unsigned char *list_data = lpNew(0);
	unsigned char *pos;
	uint64_t len;
	unsigned char int_buf[LP_INTBUF_SIZE];

	list_data = lpAppend(list_data, "list pack", 9);
	list_data = lpAppend(list_data, "-23", 3);

	list_data = lpAppend(list_data, "workflow", 8);
	list_data = lpAppend(list_data, buf, 1024);
	list_data = lpAppend(list_data, "srpc", 4);

	printf("len = %d bytes = %zu\n", lpLength(list_data), lpBytes(list_data));

	pos = lpFirst(list_data);
	// not the correct way when data is interger
	len = *(unsigned char *)pos & 0xF;
	printf("first is [%x] len = %llu data = %.*s\n", *(unsigned char *)pos, len, len, pos + 1);

	// the correct way
	unsigned char *data = lpGet(pos, &len, int_buf);
	printf("check int data len = %llu int_buf = %.*s data = %.*s\n",
			len, LP_INTBUF_SIZE, int_buf, len, data);

	int64_t count;	
	data = lpGet(pos, &count, NULL);
	printf("check again. data = %s count = %lld\n", data == NULL ? "NULL" : "NOT NULL", count);

	if (lpPrev(list_data, pos) == NULL)
		printf("already first\n");
	else
		printf("not possible\n");

	pos = lpNext(list_data, pos);

	// this is NOT the correct way
	len = *(unsigned char *)pos & 0xF;
	printf("second is [%x] len = %llu data = %.*s\n", *(unsigned char *)pos, len, len, pos + 1);

	memset(int_buf, 0, sizeof int_buf);
	// this is the correct way
	data = lpGet(pos, &len, int_buf);
	printf("check not int data len = %llu int_buf = %s data = %.*s\n",
			len, int_buf == NULL ? "NULL":"NOT NULL", len, data);

	// get next
	pos = lpNext(list_data, pos);
	len = *(unsigned char *)pos & 0xF;
	printf("long is [%x] len = %llu data = %.*s\n", *(unsigned char *)pos, len, len, pos + 1);

	// number is stored into 'int_buf' here
	data = lpGet(pos, &len, int_buf);
	printf("long data len = %llu int_buf = %.*s data = %.*s\n",
			len, LP_INTBUF_SIZE, int_buf, len, data);

	lpFree(list_data);
}

void test2()
{
	printf("\n== test2 ==\n");
//	unsigned char *lp = lpNew(0);
//	lp = lpAppendInteger(lp, 1412);
//	lp = lpReplaceInteger();
	unsigned char *list_data = lpNew(0);
	printf("len = %d bytes = %zu\n", lpLength(list_data), lpBytes(list_data));
	unsigned char *pos;
	uint64_t len;
	unsigned char int_buf[LP_INTBUF_SIZE];

	list_data = lpAppend(list_data, "-4096", 5);
	printf("len = %d bytes = %zu\n", lpLength(list_data), lpBytes(list_data));

	list_data = lpAppend(list_data, "4095", 4);
	printf("len = %d bytes = %zu\n", lpLength(list_data), lpBytes(list_data));

	pos = lpFirst(list_data);

	unsigned char *data = lpGet(pos, &len, int_buf);
	printf("check int data len = %llu int_buf = %.*s data = %.*s\n",
			len, LP_INTBUF_SIZE, int_buf, len, data);

	int64_t count;	
	data = lpGet(pos, &count, NULL);
	printf("check again. data = %s count = %lld\n", data == NULL ? "NULL" : "NOT NULL", count);
}

void test3() // test delete
{
	printf("\n== test3 ==\n");
	unsigned char *list_data = lpNew(0);
	printf("len = %d bytes = %zu\n", lpLength(list_data), lpBytes(list_data));

/*
 * unsigned char *pos, data;
 *
 * list.c: In function 'test3':
 * list.c:127:8: warning: assignment makes integer from pointer without a cast [enabled by default]
 *    data = lpGet(pos, &len, NULL);
 *
 */
	unsigned char *pos;
	uint64_t len;

	list_data = lpAppend(list_data, "WFNetworkTaskFactory1", 21);
	printf("len = %d bytes = %zu\n", lpLength(list_data), lpBytes(list_data));

//	list_data = lpAppend(list_data, "-23", 3);
	list_data = lpAppend(list_data, "WFNetworkTaskFactory2", 21);
	printf("len = %d bytes = %zu\n", lpLength(list_data), lpBytes(list_data));

	list_data = lpAppend(list_data, "WFNetworkTaskFactory3", 21);
	printf("len = %d bytes = %zu\n", lpLength(list_data), lpBytes(list_data));

	list_data = lpAppend(list_data, "WFNetworkTaskFactory4", 21);
	printf("len = %d bytes = %zu\n", lpLength(list_data), lpBytes(list_data));

	unsigned char *new_ptr = lpFirst(list_data);
	while (new_ptr)
	{
		pos = new_ptr;
		pos = lpGet(pos, &len, NULL);
		printf("get and delete data len = %ld data = %.*s\n", len, len, pos);

		list_data = lpDelete(list_data, new_ptr, &new_ptr);
	}

	printf("len = %d bytes = %zu\n", lpLength(list_data), lpBytes(list_data));

	lpFree(list_data);
}

void test4() // fake a listpack
{
	unsigned char *list_data = lpNew(0);
	printf("len = %d bytes = %zu\n", lpLength(list_data), lpBytes(list_data)); 

	unsigned char *pos = list_data;
	*pos = 1;
	printf("len = %d bytes = %zu\n", lpLength(list_data), lpBytes(list_data)); 

	pos++;
	*pos = 1;
	printf("len = %d bytes = %zu\n", lpLength(list_data), lpBytes(list_data));

	lpFree(list_data);
}

void test5() // fake a listpack data
{
	unsigned char int_buf[LP_INTBUF_SIZE];
	unsigned char *list_data = lpNew(0);

	unsigned char *pos = list_data;
	for (int i = 0; i < lpBytes(list_data); i++) {
		printf("%d(%c) ", *pos, *pos);
		pos++;
	}
	pos = lpFirst(list_data);
	printf("listpack=%x lpFirst()=%x\n\n", list_data, pos);
	
//	list_data = lpAppend(list_data, "WFNetworkTaskFactory1", 21);

	list_data = lpAppend(list_data, "A", 1);
	pos = lpFirst(list_data);
	printf("After append: len=%d bytes=%zu listpack=%x lpFirst()=%x\n\n",
			lpLength(list_data), lpBytes(list_data), list_data, pos);

	pos = list_data;
	for (int i = 0; i < lpBytes(list_data); i++) {
		printf("%d(%c) ", *pos, *pos);
		pos++;
	}
	printf("\n");

	//// append again
//	list_data = lpAppend(list_data, buf, 127);
//	list_data = lpAppend(list_data, buf, 64);
	list_data = lpAppend(list_data, buf, 63);
	printf("After append: len=%d bytes=%zu listpack=%x lpFirst()=%x\n\n",
			lpLength(list_data), lpBytes(list_data), list_data, pos);
	pos = list_data;
	for (int i = 0; i < lpBytes(list_data); i++) {
		printf("%d(%c) ", *pos, *pos);
		pos++;
	}

	///// test clear the p[6]
	pos = list_data + 6;

	pos = lpFirst(list_data);
//	*pos = 0;

	// *pos=0 dose not influent lpFirst(), but may cause lpGet failed.
	printf("listpack=%x lpFirst()=%x\n\n", list_data, pos);

	int64_t count;
	pos = lpGet(pos, &count, NULL);
	if (pos == NULL)
		printf("test failed!!!\n");
	else
		printf("len=%ld data=%.*s\n", count, count, pos);
}

int main()
{
	test1();
	test2();
	test3();
	test4();
	test5();
	return 0;
}
