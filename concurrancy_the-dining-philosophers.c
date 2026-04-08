#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5

pthread_mutex_t forks[NUM_PHILOSOPHERS];

void* philosopher(void* num) {
    int id = *(int*)num;
    int left = id;
    int right = (id + 1) % NUM_PHILOSOPHERS;

    for (int i = 0; i < 3; i++) { // Each philosopher eats 3 times
        printf("Philosopher %d is thinking...\n", id);
        sleep(1);

        // Deadlock Prevention: The "Odd" philosopher strategy
        // Philosophers 0-3 pick Left then Right. 
        // Philosopher 4 picks Right then Left.
        if (id == NUM_PHILOSOPHERS - 1) {
            pthread_mutex_lock(&forks[right]);
            pthread_mutex_lock(&forks[left]);
        } else {
            pthread_mutex_lock(&forks[left]);
            pthread_mutex_lock(&forks[right]);
        }

        printf("Philosopher %d is EATING (using forks %d and %d).\n", id, left, right);
        sleep(1);

        pthread_mutex_unlock(&forks[left]);
        pthread_mutex_unlock(&forks[right]);
        
        printf("Philosopher %d finished eating and put down forks.\n", id);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];

    // Initialize mutexes (forks)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    // Create philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, philosopher, &ids[i]);
    }

    // Wait for all philosophers to finish
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks[i]);
    }

    printf("All philosophers have finished eating.\n");
    return 0;
}


/*
Philosopher 0 is thinking...
Philosopher 1 is thinking...
Philosopher 3 is thinking...
Philosopher 4 is thinking...
Philosopher 2 is thinking...
Philosopher 1 is EATING (using forks 1 and 2).
Philosopher 3 is EATING (using forks 3 and 4).
Philosopher 3 finished eating and put down forks.
Philosopher 3 is thinking...
Philosopher 0 is EATING (using forks 0 and 1).
Philosopher 2 is EATING (using forks 2 and 3).
Philosopher 1 finished eating and put down forks.
Philosopher 1 is thinking...
Philosopher 4 is EATING (using forks 4 and 0).
Philosopher 1 is EATING (using forks 1 and 2).
Philosopher 2 finished eating and put down forks.
Philosopher 2 is thinking...
Philosopher 0 finished eating and put down forks.
Philosopher 0 is thinking...
Philosopher 1 finished eating and put down forks.
Philosopher 1 is thinking...
Philosopher 0 is EATING (using forks 0 and 1).
Philosopher 4 finished eating and put down forks.
Philosopher 4 is thinking...
Philosopher 3 is EATING (using forks 3 and 4).
Philosopher 0 finished eating and put down forks.
Philosopher 0 is thinking...
Philosopher 3 finished eating and put down forks.
Philosopher 3 is thinking...
Philosopher 4 is EATING (using forks 4 and 0).
Philosopher 2 is EATING (using forks 2 and 3).
Philosopher 2 finished eating and put down forks.
Philosopher 2 is thinking...
Philosopher 3 is EATING (using forks 3 and 4).
Philosopher 1 is EATING (using forks 1 and 2).
Philosopher 4 finished eating and put down forks.
Philosopher 4 is thinking...
Philosopher 3 finished eating and put down forks.
Philosopher 1 finished eating and put down forks.
Philosopher 2 is EATING (using forks 2 and 3).
Philosopher 0 is EATING (using forks 0 and 1).
Philosopher 2 finished eating and put down forks.
Philosopher 0 finished eating and put down forks.
Philosopher 4 is EATING (using forks 4 and 0).
Philosopher 4 finished eating and put down forks.
All philosophers have finished eating.

*/