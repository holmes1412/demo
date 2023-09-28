/*
 * gcc mt19937-64.c dict.c test_dict.c -o test_dict -I./ --std=gnu99
*/

#include "dict.h"
#include "stdio.h"
#include "string.h"

uint64_t mod = 1e9 + 7;
uint64_t cita = 997;

uint64_t hash(const void *key) {
	size_t len = strlen((char *)key);
	size_t i;
	uint64_t ret = 0;

	for (i = 0; i < len; i++)
		ret += (ret * cita + (uint64_t)((char *)key + i)) % mod;

	return ret;
}

int compare(dict *d, const void *key1, const void *key2) {
	int l1, l2;

	l1 = strlen((char *)key1);
	l2 = strlen((char *)key2);

	if (l1 != l2)
		return 0;

	return memcmp(key1, key2, l1) == 0;
}

dictType hashDictType = {
    hash,          /* hash function */
    NULL,          /* key dup */
    NULL,          /* val dup */
    compare,       /* key compare */
    NULL,          /* key destructor */
    NULL,          /* val destructor */
    NULL           /* allow to expand */
};

void basic()
{
	dict *d;
	d = dictCreate(&hashDictType);

	char *key = "a";
	char *value = "aa";
	int ret;
	dictEntry *de;

	// 1. add
	ret = dictAdd(d, key, value);
	if (ret != DICT_OK)
		fprintf(stderr, "add failed!\n");
	else
		fprintf(stderr, "add ok!\n");
	
	// 2. get
	de = dictFind(d, key);
	if (de) {
		value = dictGetVal(de);

		if (value == "aa")
			fprintf(stderr, "ok! get value 'a' = '%s'\n", value);
		else
			fprintf(stderr, "error! get value 'a' = '%s'\n", value);
	}
	else
		fprintf(stderr, "get failed!\n");

	// 3. delete
	key = "not found";
	ret = dictDelete(d, key);
	if (ret == DICT_OK)
		fprintf(stderr, "delete existed key\n");
	else
		fprintf(stderr, "delete not existed key\n");

	key = "a";
	ret = dictDelete(d, key);
	if (ret == DICT_OK)
		fprintf(stderr, "delete existed key\n");
	else
		fprintf(stderr, "delete not existed key\n");

	dictRelease(d);
}

void callback(void *privdata, const dictEntry *de)
{
	fprintf(stderr, "scan callback. get < %s , %s >\n",
			dictGetKey(de), dictGetVal(de));
//	fprintf(stderr, "privdata=%s\n", (char *)privdata);
}

void scan()
{
	dict *d;
	d = dictCreate(&hashDictType);

	char keys[7][5] = { "a", "bb", "ccc", "dddd", "e", "f", "g" };
	char *value;
	int ret = DICT_OK;
	dictEntry *de;
	int i;

	for (i = 0; i < 7 && ret == DICT_OK; i++)
		ret = dictAdd(d, keys[i], keys[i]);
	fprintf(stderr, "dict size = %d\n", dictSize(d));

 	char *context = "my context, for example, the place to put de";
	int cursor = dictScan(d, 0, callback, context);
	fprintf(stderr, "scan get cursor = %lu\n", cursor);

	while (cursor) {
		cursor = dictScan(d, cursor, callback, context);
		fprintf(stderr, "scan get cursor = %lu\n", cursor);
	}
	fprintf(stderr, "scan finished\n");

	dictRelease(d);
}

void scan_and_expand()
{
	dict *d;
	d = dictCreate(&hashDictType);

	char keys[7][5] = { "a", "bb", "ccc", "dddd", "e", "f", "g" };
	char *value;
	int ret = DICT_OK;
	dictEntry *de;
	int i;

	// add
	for (i = 0; i < 4 && ret == DICT_OK; i++)
		ret = dictAdd(d, keys[i], keys[i]);
	fprintf(stderr, "dict size = %d\n", dictSize(d));

	// scan
 	char *context = "my context, for example, the place to put de";
	int cursor = dictScan(d, 0, callback, context);
	fprintf(stderr, "scan get cursor = %lu\n", cursor);

	// continue add
	for (; i < 7 && ret == DICT_OK; i++)
		ret = dictAdd(d, keys[i], keys[i]);
	fprintf(stderr, "dict size = %d\n", dictSize(d));

	// continue scan
	while (cursor) {
		cursor = dictScan(d, cursor, callback, context);
		fprintf(stderr, "scan get cursor = %lu\n", cursor);
	}
	fprintf(stderr, "scan finished\n");

	dictRelease(d);
}

int main()
{
	scan_and_expand();
	return 0;
}

