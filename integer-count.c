#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>


#define unlock pthread_mutex_unlock
#define lock pthread_mutex_lock

pthread_mutex_t mutex;
HashTable *ht1; // space allocated inside library

void writeOutput(char *outName, int N){
	FILE *file;
	int i = 0;
	int count = 0;
	file = fopen(outName, "w");
	content_ *cur;
	
	while(i < N){
		cur = ht1->table[i];
		while(cur != NULL){
			count++;
			cur = cur->next;
		}
		i++;
	}
	
	content_ *mixedArr[count];
	int cnt2 = 0;
	i = 0;
	while(i < N){
		cur = ht1->table[i];
		while(cur != NULL){
			mixedArr[cnt2++] = cur;
			cur =cur->next;
		}
		i++;
	}
	
	// INSERTION SORT
	int j, k;
	content_ *tmp;
	for(i = 1; i < count; i++){
		tmp = mixedArr[i];
		k = tmp->key;
		
		j = i-1;
		while(j >= 0 && mixedArr[j]->key > k){
			mixedArr[j+1] = mixedArr[j];
			j--;
		}
		mixedArr[j+1] = tmp;
	}
	// IS END
	//qsort(mixedArr, count, sizeof( content_ *), comparison);

	cnt2 = 0;
	int cnt3 = 0;
	
	while(cnt2 < count)
	{
		cur = mixedArr[cnt2];
		fprintf(file,"%d", cur->key);
		fprintf(file,"%s", " : ");
		fprintf(file,"%d\n", cur->data);
		cnt3 = cnt3 + cur->data;
		cnt2++;
	}

	fclose(file);
}

void *operateFile( void *param)
{
		size_t max = 1000000;
		
		FILE *file;
		char *str = NULL;
		file = fopen(param, "r");
		if(file == NULL){
			printf("NO SUCH FILE \n");
			return -1;
		}
		int *vp;	
		while(getline(&str, &max, file) != -1){
			lock(&mutex);
			
			if(hash_get(ht1, atoi(str), &vp ) == -1){
				hash_insert(ht1, atoi(str), 1);
			}
			else{
				hash_update(ht1, atoi(str), ((int)vp)+1 );
			}
			unlock(&mutex);
		}
		fclose(file);
}

int main(int argc, char **argv)
{
	int N = 1000;
	int K = 100;
	ht1  = hash_init (N, K);
	
	pthread_mutex_init(&mutex, NULL);
	int i;
	
	int textCount = atoi(argv[1]);
	int outIdx = argc - 1;

	pthread_t threadArr[textCount];   
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int index = 0;
	for(i = 0; i < textCount; i++){
		pthread_create(&threadArr[index],&attr,operateFile,argv[i+2]);
		index++;
	}
	
	for(i= 0; i< textCount; i++){
		pthread_join(threadArr[i], NULL);
	}
	
	writeOutput(argv[outIdx], N);
	hash_destroy(ht1);
	return 0;
}
