#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "hash.h"

#define unlock pthread_mutex_unlock
#define lock pthread_mutex_lock

pthread_mutex_t mutexForDestroy = PTHREAD_MUTEX_INITIALIZER;
 

HashTable *hash_init (int N, int K)
{
	int M = N/K;
	
	if( N < MIN_N || N > MAX_N || M < MIN_M || M > MAX_M || N % M != 0){
		return (NULL);
	}
	HashTable *hashtable = NULL;
	
	//allocate table
	if( (hashtable = malloc(sizeof(HashTable))) == NULL){
		return NULL;
	}
	
	if( (hashtable->table = malloc(sizeof(content_*)*N)) == NULL){
		return NULL;
	}

	int j;
	
	for( j = 0; j < N; j++){
		hashtable->table[j] = NULL;
	}

	hashtable->sizeN =  N;
	hashtable->sizeM =  M;
	hashtable->region = K;
	hashtable->mutex = (pthread_mutex_t*) malloc(K*sizeof(pthread_mutex_t));
	
	for(j = 0; j < K; j++)
		pthread_mutex_init(&(hashtable->mutex[j]), NULL);
	
	return hashtable; 
}

int hash_insert (HashTable *hp, int k, void *v)
{	
	if(k == 0)
		return -1; // not successful
	
	int index = k % hp->sizeN;
	int regLock = index / hp->sizeM;
	
	lock(&(hp->mutex[regLock]));

	if(hp->table[index] == NULL){
		hp->table[index] = malloc(sizeof(content_*)*hp->sizeN);
		hp->table[index]->key = k;
		hp->table[index]->data = v;
		hp->table[index]->next = NULL;
	}
	else
	{
		content_ *cur = hp->table[index];
		while(cur != NULL)
		{
			if(cur-> key == k){
				unlock(&(hp->mutex[regLock]));
				return -1; // if already exists
			}
			cur = cur -> next;
		}
		content_ *new;
		content_ *temp;
		cur = hp->table[index];
		new = malloc(sizeof(content_*)*hp->sizeN);
		new->key = k;
		new->data = v;
		
		if(cur->key > new->key){
			new->next = cur;
			hp->table[index] = new;
			unlock(&(hp->mutex[regLock]));
			return 0;
		}
		while(cur ->next != NULL)
		{
			if(cur->key < new->key && cur->next->key > new->key)
			{
				temp = cur->next;
				cur->next = new;
				new->next = temp;
				unlock(&(hp->mutex[regLock]));
				return 0;
			}
			cur = cur->next;
		}
		if(cur->next == NULL){
			cur->next = new;
			new->next = NULL;
			unlock(&(hp->mutex[regLock]));
			return (0);
		}
	}
	unlock(&(hp->mutex[regLock]));
	return (0);
}

int hash_delete (HashTable *hp, int k) {
	
	if(k == 0)
		return -1; // 0 should be empty
	
	int index = k % hp->sizeN;
	int regLock = index / hp->sizeM;
	
	lock(&(hp->mutex[regLock]));
		
	content_ *cur = hp->table[index];
	content_ *temp = NULL;

	while(cur != NULL)
	{
		if(cur->key == k){
			if(cur == hp->table[index]){
				temp = hp->table[index];
				hp->table[index] = hp->table[index]->next;
				free(temp);
				temp = NULL;
			}
			else{
				temp->next = cur->next;
				free(cur);
				cur = NULL;
			}
			unlock(&(hp->mutex[regLock]));
			return 0;
		}
		temp = cur;
		cur = cur -> next;
	}
	unlock(&(hp->mutex[regLock]));
	return -1;
}

int hash_update (HashTable *hp, int k, void *v)
{
	if(k == 0)
		return -1;
	
	int index = k % hp->sizeN;
	int regLock = index / hp->sizeM;
	
	lock(&(hp->mutex[regLock]));
	
	content_ *cur = hp->table[index];
	while(cur != NULL)
	{
		if(cur->key == k)
		{
			cur -> data = v;
			unlock(&(hp->mutex[regLock]));
			return 0;
		}
		cur = cur ->next;
	}
	unlock(&(hp->mutex[regLock]));
	return -1;

}


int hash_get (HashTable *hp, int k, void **vp)
{	
	if(k == 0)
		return -1;
	
	int index = k % hp->sizeN;
	int regLock = index / hp->sizeM;
	
	lock(&(hp->mutex[regLock]));
	
	content_ *cur = hp->table[index];
	while(cur != NULL)
	{
		if(cur->key == k)
		{
			vp[0] = cur -> data;
			unlock(&(hp->mutex[regLock]));
			return 0;
		}
		cur = cur ->next;
	}
	unlock(&(hp->mutex[regLock]));
	return -1;
}

int hash_destroy (HashTable *hp)
{
	lock(&mutexForDestroy);
	content_ *cur;
	content_ *temp = NULL;
	int i;
	for(i = 0; i < hp->sizeN; i++)
	{
		if(hp->table[i] != NULL){
			cur = hp->table[i];
		}
		else{
			cur = NULL;
		}
		while(cur != NULL){
			hash_delete(hp, hp->table[i]->key);
			cur = hp->table[i];
		}
		
	}
	free(hp->mutex);
	unlock(&mutexForDestroy);
	return (0); 
}
