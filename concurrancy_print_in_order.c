/*
Suppose we have a class:

public class Foo {
  public void first() { print("first"); }
  public void second() { print("second"); }
  public void third() { print("third"); }
}
The same instance of Foo will be passed to three different threads. Thread A will call first(), thread B will call second(), and thread C will call third(). Design a mechanism and modify the program to ensure that second() is executed after first(), and third() is executed after second().

Note:

We do not know how the threads will be scheduled in the operating system, even though the numbers in the input seem to imply the ordering. The input format you see is mainly to ensure our tests' comprehensiveness.

 

Example 1:

Input: nums = [1,2,3]
Output: "firstsecondthird"
Explanation: There are three threads being fired asynchronously. The input [1,2,3] means thread A calls first(), thread B calls second(), and thread C calls third(). "firstsecondthird" is the correct output.
Example 2:

Input: nums = [1,3,2]
Output: "firstsecondthird"
Explanation: The input [1,3,2] means thread A calls first(), thread B calls third(), and thread C calls second(). "firstsecondthird" is the correct output.
 

Constraints:

nums is a permutation of [1, 2, 3].

*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// --- LeetCode Style Wrapper Functions ---
void printFirst()  { printf("first");  }
void printSecond() { printf("second"); }
void printThird()  { printf("third");  }

// --- The Foo Class Implementation ---
typedef struct {
    sem_t sem2;
    sem_t sem3;
} Foo;

Foo* fooCreate() {
    Foo* obj = (Foo*) malloc(sizeof(Foo));
    sem_init(&obj->sem2, 0, 0);
    sem_init(&obj->sem3, 0, 0);
    return obj;
}

void first(Foo* obj) {
    printFirst();
    sem_post(&obj->sem2);
}

void second(Foo* obj) {
    sem_wait(&obj->sem2);
    printSecond();
    sem_post(&obj->sem3);
}

void third(Foo* obj) {
    sem_wait(&obj->sem3);
    printThird();
}

void fooFree(Foo* obj) {
    sem_destroy(&obj->sem2);
    sem_destroy(&obj->sem3);
    free(obj);
}

// --- Thread Entry Points ---
void* threadA(void* arg) {
    first((Foo*)arg);
    return NULL;
}

void* threadB(void* arg) {
    second((Foo*)arg);
    return NULL;
}

void* threadC(void* arg) {
    third((Foo*)arg);
    return NULL;
}

// --- Execution Orchestrator ---
int main() {
    Foo* obj = fooCreate();
    pthread_t t1, t2, t3;

    // We start the threads in a random order to prove the synchronization works.
    // Even if Thread C starts first, it will wait for the others.
    pthread_create(&t3, NULL, threadC, obj);
    pthread_create(&t2, NULL, threadB, obj);
    pthread_create(&t1, NULL, threadA, obj);

    // Wait for all threads to finish
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    printf("\nDone.\n");

    fooFree(obj);
    return 0;
}

/*
firstsecondthird
Done.
*/