/*
You have a function printNumber that can be called with an integer parameter and prints it to the console.

For example, calling printNumber(7) prints 7 to the console.
You are given an instance of the class ZeroEvenOdd that has three functions: zero, even, and odd. The same instance of ZeroEvenOdd will be passed to three different threads:

Thread A: calls zero() that should only output 0's.
Thread B: calls even() that should only output even numbers.
Thread C: calls odd() that should only output odd numbers.
Modify the given class to output the series "010203040506..." where the length of the series must be 2n.

Implement the ZeroEvenOdd class:

ZeroEvenOdd(int n) Initializes the object with the number n that represents the numbers that should be printed.
void zero(printNumber) Calls printNumber to output one zero.
void even(printNumber) Calls printNumber to output one even number.
void odd(printNumber) Calls printNumber to output one odd number.
 

Example 1:

Input: n = 2
Output: "0102"
Explanation: There are three threads being fired asynchronously.
One of them calls zero(), the other calls even(), and the last one calls odd().
"0102" is the correct output.
Example 2:

Input: n = 5
Output: "0102030405"

*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

// Mock printNumber function as specified
void printNumber(int val) {
    printf("%d", val);
}

typedef struct {
    int n;
    sem_t zeroSem;
    sem_t oddSem;
    sem_t evenSem;
} ZeroEvenOdd;

ZeroEvenOdd* create(int n) {
    ZeroEvenOdd* obj = (ZeroEvenOdd*) malloc(sizeof(ZeroEvenOdd));
    obj->n = n;
    sem_init(&obj->zeroSem, 0, 1); // Zero goes first
    sem_init(&obj->oddSem, 0, 0);
    sem_init(&obj->evenSem, 0, 0);
    return obj;
}

void* zero(void* arg) {
    ZeroEvenOdd* obj = (ZeroEvenOdd*)arg;
    for (int i = 1; i <= obj->n; i++) {
        sem_wait(&obj->zeroSem);
        printNumber(0);
        
        // If the next number to print is odd, wake Odd thread; else wake Even
        if (i % 2 != 0) {
            sem_post(&obj->oddSem);
        } else {
            sem_post(&obj->evenSem);
        }
    }
    return NULL;
}

void* odd(void* arg) {
    ZeroEvenOdd* obj = (ZeroEvenOdd*)arg;
    for (int i = 1; i <= obj->n; i += 2) {
        sem_wait(&obj->oddSem);
        printNumber(i);
        sem_post(&obj->zeroSem);
    }
    return NULL;
}

void* even(void* arg) {
    ZeroEvenOdd* obj = (ZeroEvenOdd*)arg;
    for (int i = 2; i <= obj->n; i += 2) {
        sem_wait(&obj->evenSem);
        printNumber(i);
        sem_post(&obj->zeroSem);
    }
    return NULL;
}

void destroy(ZeroEvenOdd* obj) {
    sem_destroy(&obj->zeroSem);
    sem_destroy(&obj->oddSem);
    sem_destroy(&obj->evenSem);
    free(obj);
}

int main() {
    int n = 5; // Example input
    ZeroEvenOdd* obj = create(n);

    pthread_t tA, tB, tC;

    // Launching threads in non-sequential order to test synchronization
    pthread_create(&tB, NULL, even, obj);
    pthread_create(&tC, NULL, odd, obj);
    pthread_create(&tA, NULL, zero, obj);

    pthread_join(tA, NULL);
    pthread_join(tB, NULL);
    pthread_join(tC, NULL);

    printf("\n");
    destroy(obj);
    return 0;
}


/*

gcc zero_even_odd.c -o zero_even_odd -lpthread

0102030405

*/