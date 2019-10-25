#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include <time.h>
#include <sys/time.h>

#define unlock pthread_mutex_unlock
#define lock pthread_mutex_lock

HashTable *ht1; // space allocated inside library
pthread_mutex_t mutex;

void *keyExperiment(void *param){
	double timePassed;
	struct timeval startTime, endTime;
	int i;
	
	gettimeofday(&startTime, NULL);
	
	ht1  = hash_init (1000, 100);
	for (i = 0; i < param; ++i) {
	   hash_insert (ht1, i, (void *) 35000); 
	}
	hash_destroy (ht1); 

	gettimeofday(&endTime, NULL);

	timePassed = (endTime.tv_sec - startTime.tv_sec) * 1000.0;
	timePassed += (endTime.tv_usec - startTime.tv_usec) / 1000.0;
	printf("Time passed for ");
	printf("%d", param);
	printf(" keys: ");
	printf("%4f\n", timePassed);
}

void *tableSizeExperiment(void *param){
	double timePassed;
	struct timeval startTime, endTime;
	int i;
	
	gettimeofday(&startTime, NULL);
	
	ht1  = hash_init (1000, 10);
	for (i = 0; i < 1000; ++i) {
	   hash_insert (ht1, i, (void *) 35000); 
	}
	hash_destroy (ht1); 

	gettimeofday(&endTime, NULL);

	timePassed = (endTime.tv_sec - startTime.tv_sec) * 1000.0;
	timePassed += (endTime.tv_usec - startTime.tv_usec) / 1000.0;
	printf("Time passed for ");
	printf("%d", param);
	printf(" as table size: ");
	printf("%4f\n", timePassed); 
}

void *lockSizeExperiment(void *param){
	double timePassed;
	struct timeval startTime, endTime;
	int i;
	
	gettimeofday(&startTime, NULL);
	
	ht1  = hash_init (1000, param);
	for (i = 0; i < 1000; ++i) {
	   hash_insert (ht1, i, (void *) 35000); 
	}
	hash_destroy (ht1); 

	gettimeofday(&endTime, NULL);

	timePassed = (endTime.tv_sec - startTime.tv_sec) * 1000.0;
	timePassed += (endTime.tv_usec - startTime.tv_usec) / 1000.0;
	printf("Time passed for ");
	printf("%d", param);
	printf(" locks: ");
	printf("%4f\n", timePassed); 
}

void *threadExperiment(void *param){
	lock(&mutex);
	int i;
	
	ht1  = hash_init (1000, 100);
	for (i = 0; i < 1000; ++i) {
	   hash_insert (ht1, i, (void *) 35000); 
	}
	hash_destroy (ht1); 
	unlock(&mutex);
}

int main(int argc, char **argv)
{
	int i, j;
	int W, N, K;
	int threadCount = 10; // T
	pthread_t threadArr[threadCount];   
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_mutex_init(&mutex, NULL);
	
	
	// Experiment For Keys (W) //keyler 10000den 100000e kadar
	printf("NUMBER OF KEYS EXPERIMENT \n");
	for(i = 1; i <= 10; i++)
	{
		W = i * 10000;
		pthread_create(&threadArr[0],&attr,keyExperiment,W);
		pthread_join(threadArr[0], NULL);
	}
	printf("\n");
	
	// Experiment For Table Size (N)  N 100den 1000e kadar 100er 100er
	printf("TABLE SIZE EXPERIMENT \n");
	for(i = 1; i <= 10; i++)
	{
		N = i * 100;
		pthread_create(&threadArr[0],&attr,tableSizeExperiment,N);
		pthread_join(threadArr[0], NULL);
	}
	printf("\n");
	
	// Experiment For Lock Size (K)
	printf("NUMBER OF LOCKS EXPERIMENT \n");
	for(i = 1; i <= 5; i++)
	{
		if(i ==1) K = 1;
		else if(i ==2) K = 10;
		else if(i ==3) K = 20;
		else if(i ==4) K = 50;
		else if(i ==5) K = 100;
		
		pthread_create(&threadArr[0],&attr,lockSizeExperiment,K);
		pthread_join(threadArr[0], NULL);
	}
	printf("\n");
	
	
	// Experiment For Thread Size (T)
	printf("NUMBER OF THREADS EXPERIMENT \n");
	int count;
	// THREAD SIZE = 1
	
	double timePassed;
	struct timeval startTime, endTime;
	
	for(count = 1; count <= 10; count++)
	{
		gettimeofday(&startTime, NULL);

		for(i = 0; i < count; i++)
			pthread_create(&threadArr[i],&attr,threadExperiment,count);
		for(i= 0; i< count; i++)
			pthread_join(threadArr[i], NULL);
		
		gettimeofday(&endTime, NULL);
		
		timePassed = (endTime.tv_sec - startTime.tv_sec) * 1000.0;
		timePassed += (endTime.tv_usec - startTime.tv_usec) / 1000.0;
		printf("Time passed for ");
		printf("%d", count);
		printf(" thread: ");
		printf("%4f\n", timePassed); 
	}	
		
	return 0;
}
