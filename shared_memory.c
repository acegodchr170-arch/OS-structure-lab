#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

#define SHM_SIZE 1024

int main() {
    key_t key = ftok("shmfile", 65);
    
    // 1. Create a shared memory segment
    int shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid < 0) {
        perror("shmget failed");
        exit(1);
    }

    // Initialize an unnamed POSIX semaphore in shared memory format or use a named one for fork
    sem_t *sem = sem_open("/shm_sem", O_CREAT, 0666, 0);

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }
    else if (pid == 0) { // Child Process (Reader)
        // 2. Attach memory
        char *str = (char*) shmat(shmid, (void*)0, 0);
        
        printf("Child: Waiting for Parent to write data...\n");
        sem_wait(sem); // Wait until parent signals data is ready
        
        printf("Child Read from Shared Memory: %s\n", str);
        
        // 5. Detach memory
        shmdt(str);
        sem_close(sem);
        exit(0);
    } 
    else { // Parent Process (Writer)
        // 2. Attach memory
        char *str = (char*) shmat(shmid, (void*)0, 0);
        
        sleep(1); // Simulate calculation time
        printf("Parent: Writing data to Shared Memory...\n");
        sprintf(str, "Hello from the Parent Process! IPC Successful.");
        
        sem_post(sem); // Notify child
        
        // Wait for child to finish reading
        wait(NULL);
        
        // 5. Clean up
        shmdt(str);
        shmctl(shmid, IPC_RMID, NULL);
        sem_close(sem);
        sem_unlink("/shm_sem");
        printf("Parent: Shared memory segment cleaned up successfully.\n");
    }

    return 0;
}
