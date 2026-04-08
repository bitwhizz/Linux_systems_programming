/*
There are two kinds of threads: oxygen and hydrogen. Your goal is to group these threads to form water molecules.

There is a barrier where each thread has to wait until a complete molecule can be formed. Hydrogen and oxygen threads will be given releaseHydrogen and releaseOxygen methods respectively, which will allow them to pass the barrier. These threads should pass the barrier in groups of three, and they must immediately bond with each other to form a water molecule. You must guarantee that all the threads from one molecule bond before any other threads from the next molecule do.

In other words:

If an oxygen thread arrives at the barrier when no hydrogen threads are present, it must wait for two hydrogen threads.
If a hydrogen thread arrives at the barrier when no other threads are present, it must wait for an oxygen thread and another hydrogen thread.
We do not have to worry about matching the threads up explicitly; the threads do not necessarily know which other threads they are paired up with. The key is that threads pass the barriers in complete sets; thus, if we examine the sequence of threads that bind and divide them into groups of three, each group should contain one oxygen and two hydrogen threads.

Write synchronization code for oxygen and hydrogen molecules that enforces these constraints.

 

Example 1:

Input: water = "HOH"
Output: "HHO"
Explanation: "HOH" and "OHH" are also valid answers.
Example 2:

Input: water = "OOHHHH"
Output: "HHOHHO"
Explanation: "HOHHHO", "OHHHHO", "HHOHOH", "HOHHOH", "OHHHOH", "HHOOHH", "HOHOHH" and "OHHOHH" are also valid answers.
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Mock release functions
void releaseHydrogen() { printf("H"); }
void releaseOxygen()   { printf("O"); }

typedef struct {
    sem_t semH;
    sem_t semO;
    pthread_barrier_t barrier;
} H2O;

H2O* h2oCreate() {
    H2O* obj = (H2O*) malloc(sizeof(H2O));
    // Allow 2 H and 1 O at a time
    sem_init(&obj->semH, 0, 2);
    sem_init(&obj->semO, 0, 1);
    // Barrier waits for exactly 3 threads
    pthread_barrier_init(&obj->barrier, NULL, 3);
    return obj;
}

void* hydrogen(void* arg) {
    H2O* obj = (H2O*)arg;
    
    sem_wait(&obj->semH);
    // Wait for 2 H and 1 O to reach this point
    pthread_barrier_wait(&obj->barrier);
    
    releaseHydrogen();
    
    // Once the barrier is crossed by all 3, the last one resets the flow
    // We use a small trick: the 3rd thread crossing the barrier releases permits
    // but in pthreads, we can just release permits after the work.
    sem_post(&obj->semH); 
    return NULL;
}

void* oxygen(void* arg) {
    H2O* obj = (H2O*)arg;
    
    sem_wait(&obj->semO);
    pthread_barrier_wait(&obj->barrier);
    
    releaseOxygen();
    
    sem_post(&obj->semO);
    return NULL;
}

int main() {
    char* input = "OOHHHH"; // Example 2
    int len = strlen(input);
    H2O* obj = h2oCreate();
    
    pthread_t threads[len];

    // Create threads based on input string
    for (int i = 0; i < len; i++) {
        if (input[i] == 'H') {
            pthread_create(&threads[i], NULL, hydrogen, obj);
        } else {
            pthread_create(&threads[i], NULL, oxygen, obj);
        }
    }

    for (int i = 0; i < len; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nDone.\n");
    return 0;
}

/*
OHHHOH
Done.
*/