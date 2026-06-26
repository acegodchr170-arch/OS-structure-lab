#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 5
#define INCREMENTS_PER_THREAD 100000

int shared_counter = 0;
pthread_mutex_t lock;

// Thread function WITHOUT Mutex
void* increment_without_mutex(void* arg) {
    for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
        shared_counter++; // Race condition occurs here
    }
    return NULL;
}

// Thread function WITH Mutex
void* increment_with_mutex(void* arg) {
    for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
        pthread_mutex_lock(&lock);
        shared_counter++; // Protected critical section
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    // --- TEST 1: WITHOUT MUTEX ---
    shared_counter = 0;
    printf("Running WITHOUT Mutex...\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment_without_mutex, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Expected Counter: %d\n", NUM_THREADS * INCREMENTS_PER_THREAD);
    printf("Actual Counter:   %d\n", shared_counter);
    printf("Result: %s\n\n", (shared_counter == NUM_THREADS * INCREMENTS_PER_THREAD) ? "SUCCESS" : "FAIL (Race Condition Detected!)");

    // --- TEST 2: WITH MUTEX ---
    shared_counter = 0;
    pthread_mutex_init(&lock, NULL);
    printf("Running WITH Mutex...\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment_with_mutex, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Expected Counter: %d\n", NUM_THREADS * INCREMENTS_PER_THREAD);
    printf("Actual Counter:   %d\n", shared_counter);
    printf("Result: %s\n", (shared_counter == NUM_THREADS * INCREMENTS_PER_THREAD) ? "SUCCESS (Thread Safe)" : "FAIL");

    pthread_mutex_destroy(&lock);
    return 0;
}
