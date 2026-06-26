#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TOTAL_THREADS 6
#define MAX_CONCURRENT_ACCESS 3

sem_t counting_sem;

void* resource_access_thread(void* arg) {
    long id = (long)arg;

    printf("Thread %ld is waiting to access the resource pool...\n", id);
    sem_wait(&counting_sem);

    // Critical Section - Multiple threads allowed here up to MAX_CONCURRENT_ACCESS
    printf("-> Thread %ld has GRANTED access to the resource pool.\n", id);
    sleep(2); // Simulating resource usage
    printf("<- Thread %ld is leaving the resource pool.\n", id);

    sem_post(&counting_sem);
    return NULL;
}

int main() {
    pthread_t threads[TOTAL_THREADS];
    
    // Initialize semaphore to allow 3 threads simultaneously
    sem_init(&counting_sem, 0, MAX_CONCURRENT_ACCESS);
    
    printf("Starting simulation. (Max concurrent threads allowed: %d)\n\n", MAX_CONCURRENT_ACCESS);

    for (long i = 0; i < TOTAL_THREADS; i++) {
        pthread_create(&threads[i], NULL, resource_access_thread, (void*)i);
    }

    for (int i = 0; i < TOTAL_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&counting_sem);
    
    printf("\nDiscussion Answer: Counting semaphores are used instead of mutexes when managing a finite pool of identical resources (e.g., connection pools, hardware devices) rather than guarding a single block of data.\n");
    return 0;
}
