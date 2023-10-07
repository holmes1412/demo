/*
 This is a demo refer to redis/dict.h, and support serialize/deserialize.
*/

#ifndef __HASH_TABLE_H
#define __HASH_TABLE_H

typedef struct hashEntry hashEntry; /* opaque */

typedef struct hash hash;

#define HT_SIZE(exp) ((unsigned long)1<<(exp))
#define HT_SIZE_MASK(exp) ((HT_SIZE(exp))-1)

struct hash {
	hashEntry **ht_table;
	unsigned int ht_used;
	signed char ht_size_exp;

	unsigned long (*hash_function)(const void *key, unsigned int len);
    int (*key_compare)(const void *key1, unsigned int len1,
					   const void *key2, unsigned int len2);
//    void *(*dup)(hash *d, const void *buf, unsigned int size);
//    void (*destructor)(hash *d, void *buf, unsigned int size);
	// metadata
};

typedef struct hashIterator {
	//TODO:
} hashIterator;

typedef void (hashScanFunction)(void *privdata, const hashEntry *he);

/* This is the initial size of every hash table */
#define HT_INITIAL_EXP      2
#define HT_INITIAL_SIZE     (1<<(HT_INITIAL_EXP))

#define hashSlots(h) (HT_SIZE((h)->ht_size_exp))
#define hashSize(h) ((h)->ht_used)

/////////////// API /////////////////

hash *hashCreate();
int hashExpand(hash *h, unsigned int size);
int hashAdd(hash *h, void *key, unsigned int ksiz, void *val, unsigned int vsiz);
int hashReplace(hash *h, void *key, void *val);
int hashAddRaw(hash *h, void *key, hashEntry **existing);
int hashDelete(hash *h, const void *key, unsigned int ksiz);
void hashRelease(hash *h);
hashEntry *hashFind(hash *h, const void *key);
void *hashGetKey(const hashEntry *he);
void *hashGetVal(const hashEntry *he);
unsigned int hashMemUsed(const hash *h);

unsigned int hashScan(hash *h, unsigned int v, hashScanFunction *fn, void *privdata);

#endif
