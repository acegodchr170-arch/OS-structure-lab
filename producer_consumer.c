#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

sem_t mutex_sem;
sem_t empty;
sem_t full;

void* producer(void* arg) {
    for (int i = 1; i <= 10; i++) {
        sleep(1); // Simulate work / varying speeds
        
        sem_wait(&empty);
        sem_wait(&mutex_sem);

        // Critical Section
        buffer[in] = i;
        printf("Producer produced item %d at index %d\n", i, in);
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&mutex_sem);
        sem_post(&full);
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 1; i <= 10; i++) {
        sleep(2); // Consumer operates at a different speed
        
        sem_wait(&full);
        sem_wait(&mutex_sem);

        // Critical Section
        int item = buffer[out];
        printf("Consumer consumed item %d from index %d\n", item, out);
        out = (out + 1) % BUFFER_SIZE;

        sem_post(&mutex_sem);
        sem_post(&empty);
    }
    return NULL;
}

int main() {
    pthread_t prod_thread, cons_thread;

    sem_init(&mutex_sem, 0, 1);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    sem_destroy(&mutex_sem);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
