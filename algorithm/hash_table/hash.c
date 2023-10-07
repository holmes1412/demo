/*
 Use the default stradegy:
 expand once the used entry == hash_table size
*/

#include "hash.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

///////////////// types ////////////////

struct hashEntry {
	void *key;
	void *val;
	struct hashEntry *next;
};

// TODO: This is test version. Will refer to encoding like mysql string.
void __get_obj(const void *obj, void **buf, unsigned int *len)
{
	*len = *((unsigned int *)obj);
	*buf = (char *)obj + sizeof(unsigned int);
}

// TODO: This can be optimized for nocopy version
int __set_obj(const void *buf, unsigned int len, void **obj)
{
	unsigned char *ptr = malloc(sizeof(unsigned int) + len);
	if (ptr == NULL)
		return -1;

	memcpy(ptr, &len, sizeof(unsigned int));
	memcpy(ptr + sizeof(unsigned int), buf, len);
	*obj = ptr;
	return 0;
}

///////////////// default function /////////////////

unsigned long mod = 1e9 + 7;
unsigned long cita = 997;

unsigned long __default_hash_function(const void *key, unsigned int len)
{
//	unsigned int len = strlen((char *)key);
	unsigned int i;
	unsigned long ret = 0;

	for (i = 0; i < len; i++)
		ret += (ret * cita + (unsigned long)((char *)key + i)) % mod;

	return ret;
}

int __default_key_compare(const void *key1, unsigned int l1,
						  const void *key2, unsigned int l2)
{
/*
	int l1, l2;

	l1 = strlen((char *)key1);
	l2 = strlen((char *)key2);
*/
	if (l1 != l2)
		return 0;

	return memcmp(key1, key2, l1) == 0;
}

///////////////// API impl ////////////////

hash *hashCreate()
{
	hash *h = malloc(sizeof(*h));
	if (h == NULL)
		return NULL;

	h->ht_used = 0;
	h->ht_size_exp = HT_INITIAL_EXP;
	h->ht_table = calloc(0, HT_INITIAL_EXP*sizeof(hashEntry*));
//	h->ht_table = calloc(1, HT_INITIAL_EXP*sizeof(hashEntry*));

	h->hash_function = __default_hash_function;
	h->key_compare = __default_key_compare;
//	h->dup = NULL;
//	h->destruct = NULL;

	return h;
}

/*
 return
 -1 : no need to expand
  0 : success
*/
int hashExpand(hash *h, unsigned int size)
{
	if (h->ht_used > size)
		return -1;

	// TODO
	return 0;
}

/*
 return
 -1 : failed
  0 : new add
  1 : exists and overwrite
*/
int hashAdd(hash *h, void *key, unsigned int ksiz, void *val, unsigned int vsiz)
{
	unsigned int idx;
	hashEntry *entry;
	hashEntry *prev = NULL;
	void *buf;
	unsigned int len;
	unsigned long hash = h->hash_function(key, ksiz);
	int ret = 0;

	// 1. ExpandIfNeeded
	//    can be optimized: if add an existed kv
	if (h->ht_used >= HT_SIZE(h->ht_size_exp))
		hashExpand(h, h->ht_used + 1);

	// 2. FindPositionForInsert
	idx = hash & HT_SIZE_MASK(h->ht_size_exp);
	entry = h->ht_table[idx];

	while (entry) {
		__get_obj(entry->key, &buf, &len);
		if (h->key_compare(buf, len, key, ksiz) == 0) {
			ret = 1;
			break;
		}

		prev = entry;
		entry = entry->next;
	}

	// 3. InsertAtPosition
	if (ret == 1) {
		void *origin_val = entry->val;
		if (__set_obj(val, vsiz, &entry->val))
			return -1;
		free(origin_val);

	} else {
		entry = malloc(sizeof(hashEntry));

		if (__set_obj(key, ksiz, &entry->key)) {
			free(entry);
			return -1;
		}

		if (__set_obj(val, vsiz, &entry->val)) {
			free(entry->key);
			free(entry);
		}

		entry->next = prev;
		h->ht_table[idx] = entry;
	}

	h->ht_used++;
	return ret;
}

/*
 return
  0 : deleted
  1 : not found
*/
int hashDelete(hash *h, const void *key, unsigned int ksiz)
{
	hashEntry *entry, *prev = NULL;
	void *buf;
	unsigned int len;

	if (hashSize(h) == 0) return 1;

	unsigned long hash = h->hash_function(key, ksiz);
	unsigned long idx = hash & HT_SIZE_MASK(h->ht_size_exp);
	entry = h->ht_table[idx];

	while (entry) {
		__get_obj(entry->key, &buf, &len);
		if (h->key_compare(buf, len, key, ksiz) == 0) {
			free(entry->key);
			free(entry->val);

			if (prev)
				prev->next = entry->next;
			else
				h->ht_table[idx] = entry->next;

			h->ht_used--;
			return 0;
		}

		prev = entry;
		entry = entry->next;
	}

	return 1;
}

void hashRelease(hash *h)
{
	if (!h)
		return;
	
    unsigned long i;
	hashEntry *entry, *next;

	for (i = 0; i < HT_SIZE(h->ht_size_exp) && h->ht_used > 0; i++) {
		entry = h->ht_table[i];
		if (entry == NULL)
			continue;

		while (entry) {
			next = entry->next;
			free(entry->key);
			free(entry->val);
			h->ht_used--;
			entry = next;
		}

	}

	free(h->ht_table);
	free(h);
}

hashEntry *hashFind(hash *h, const void *key)
{
	return NULL;
}

void *hashGetKey(const hashEntry *entry)
{
	return NULL;
}

void *hashGetVal(const hashEntry *entry)
{
	return NULL;
}

unsigned int hashMemUsed(const hash *h)
{
	return hashSize(h) * sizeof(hashEntry) + hashSlots(h) * sizeof(hashEntry*);
}

unsigned int hashScan(hash *h, unsigned int v, hashScanFunction *fn, void *privdata)
{
	return 0;
}

