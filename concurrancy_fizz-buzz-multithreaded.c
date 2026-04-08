/*

You have the four functions:

printFizz that prints the word "fizz" to the console,
printBuzz that prints the word "buzz" to the console,
printFizzBuzz that prints the word "fizzbuzz" to the console, and
printNumber that prints a given integer to the console.
You are given an instance of the class FizzBuzz that has four functions: fizz, buzz, fizzbuzz and number. The same instance of FizzBuzz will be passed to four different threads:

Thread A: calls fizz() that should output the word "fizz".
Thread B: calls buzz() that should output the word "buzz".
Thread C: calls fizzbuzz() that should output the word "fizzbuzz".
Thread D: calls number() that should only output the integers.
Modify the given class to output the series [1, 2, "fizz", 4, "buzz", ...] where the ith token (1-indexed) of the series is:

"fizzbuzz" if i is divisible by 3 and 5,
"fizz" if i is divisible by 3 and not 5,
"buzz" if i is divisible by 5 and not 3, or
i if i is not divisible by 3 or 5.
Implement the FizzBuzz class:

FizzBuzz(int n) Initializes the object with the number n that represents the length of the sequence that should be printed.
void fizz(printFizz) Calls printFizz to output "fizz".
void buzz(printBuzz) Calls printBuzz to output "buzz".
void fizzbuzz(printFizzBuzz) Calls printFizzBuzz to output "fizzbuzz".
void number(printNumber) Calls printnumber to output the numbers.
 

Example 1:

Input: n = 15
Output: [1,2,"fizz",4,"buzz","fizz",7,8,"fizz","buzz",11,"fizz",13,14,"fizzbuzz"]
Example 2:

Input: n = 5
Output: [1,2,"fizz",4,"buzz"]
 

*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

// Mock print functions
void printFizz()     { printf("fizz, "); }
void printBuzz()     { printf("buzz, "); }
void printFizzBuzz() { printf("fizzbuzz, "); }
void printNumber(int x) { printf("%d, ", x); }

typedef struct {
    int n;
    int curr;
    sem_t sem_f;
    sem_t sem_b;
    sem_t sem_fb;
    sem_t sem_num;
} FizzBuzz;

FizzBuzz* create(int n) {
    FizzBuzz* obj = (FizzBuzz*) malloc(sizeof(FizzBuzz));
    obj->n = n;
    obj->curr = 1;
    sem_init(&obj->sem_f, 0, 0);
    sem_init(&obj->sem_b, 0, 0);
    sem_init(&obj->sem_fb, 0, 0);
    sem_init(&obj->sem_num, 0, 0);
    return obj;
}

// Helper to signal the correct thread for the current 'curr'
void releaseNext(FizzBuzz* obj) {
    if (obj->curr > obj->n) {
        // Signal all to exit
        sem_post(&obj->sem_f);
        sem_post(&obj->sem_b);
        sem_post(&obj->sem_fb);
        sem_post(&obj->sem_num);
        return;
    }
    if (obj->curr % 3 == 0 && obj->curr % 5 == 0) sem_post(&obj->sem_fb);
    else if (obj->curr % 3 == 0) sem_post(&obj->sem_f);
    else if (obj->curr % 5 == 0) sem_post(&obj->sem_b);
    else sem_post(&obj->sem_num);
}

void* fizz(void* arg) {
    FizzBuzz* obj = (FizzBuzz*)arg;
    while (1) {
        sem_wait(&obj->sem_f);
        if (obj->curr > obj->n) break;
        printFizz();
        obj->curr++;
        releaseNext(obj);
    }
    return NULL;
}

void* buzz(void* arg) {
    FizzBuzz* obj = (FizzBuzz*)arg;
    while (1) {
        sem_wait(&obj->sem_b);
        if (obj->curr > obj->n) break;
        printBuzz();
        obj->curr++;
        releaseNext(obj);
    }
    return NULL;
}

void* fizzbuzz(void* arg) {
    FizzBuzz* obj = (FizzBuzz*)arg;
    while (1) {
        sem_wait(&obj->sem_fb);
        if (obj->curr > obj->n) break;
        printFizzBuzz();
        obj->curr++;
        releaseNext(obj);
    }
    return NULL;
}

void* number(void* arg) {
    FizzBuzz* obj = (FizzBuzz*)arg;
    while (1) {
        sem_wait(&obj->sem_num);
        if (obj->curr > obj->n) break;
        printNumber(obj->curr);
        obj->curr++;
        releaseNext(obj);
    }
    return NULL;
}

int main() {
    int n = 15;
    FizzBuzz* obj = create(n);

    pthread_t tA, tB, tC, tD;

    pthread_create(&tA, NULL, fizz, obj);
    pthread_create(&tB, NULL, buzz, obj);
    pthread_create(&tC, NULL, fizzbuzz, obj);
    pthread_create(&tD, NULL, number, obj);

    // Initial kickstart
    releaseNext(obj);

    pthread_join(tA, NULL);
    pthread_join(tB, NULL);
    pthread_join(tC, NULL);
    pthread_join(tD, NULL);

    printf("\nDone.\n");
    return 0;
}


/*
1, 2, fizz, 4, buzz, fizz, 7, 8, fizz, buzz, 11, fizz, 13, 14, fizzbuzz, 
Done.

*/