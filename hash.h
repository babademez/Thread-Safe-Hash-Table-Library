#ifndef HASH_H
#define HASH_H

#include <pthread.h>

#define MIN_N 100
#define MAX_N 1000
#define MIN_M 10
#define MAX_M 1000

struct content{
	int key;
	void* data;
	struct content *next;
	
	
};

struct hash_table
{
	// put your table definition here
	int sizeN; //size of the array
	int sizeM; // size of m 
	int region;
	pthread_mutex_t *mutex;
	struct content **table;
};

typedef struct hash_table HashTable;
typedef struct content content_; 

HashTable *hash_init (int N, int K);
int hash_insert (HashTable *hp, int k, void* v);
int hash_delete (HashTable *hp, int k);
int hash_update (HashTable *hp, int, void *v); 
int hash_get (HashTable *hp, int k, void **vp);
int hash_destroy (HashTable *hp); 


#endif /* HASH_H */
