/*
Suppose you are given the following code:

class FooBar {
  public void foo() {
    for (int i = 0; i < n; i++) {
      print("foo");
    }
  }

  public void bar() {
    for (int i = 0; i < n; i++) {
      print("bar");
    }
  }
}
The same instance of FooBar will be passed to two different threads:

thread A will call foo(), while
thread B will call bar().
Modify the given program to output "foobar" n times.

 

Example 1:

Input: n = 1
Output: "foobar"
Explanation: There are two threads being fired asynchronously. One of them calls foo(), while the other calls bar().
"foobar" is being output 1 time.
Example 2:

Input: n = 2
Output: "foobarfoobar"
Explanation: "foobar" is being output 2 times.
 

Constraints:

1 <= n <= 1000

*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

// Simple wrappers for the print functions
void printFoo() { printf("foo"); }
void printBar() { printf("bar"); }

typedef struct {
    int n;
    sem_t fooSem;
    sem_t barSem;
} FooBar;

FooBar* fooBarCreate(int n) {
    FooBar* obj = (FooBar*) malloc(sizeof(FooBar));
    obj->n = n;
    // fooSem starts at 1 so foo() can go first
    sem_init(&obj->fooSem, 0, 1);
    // barSem starts at 0 so bar() must wait
    sem_init(&obj->barSem, 0, 0);
    return obj;
}

void* foo(void* arg) {
    FooBar* obj = (FooBar*)arg;
    for (int i = 0; i < obj->n; i++) {
        sem_wait(&obj->fooSem); // Wait for permission to print foo
        printFoo();
        sem_post(&obj->barSem); // Signal bar() to print
    }
    return NULL;
}

void* bar(void* arg) {
    FooBar* obj = (FooBar*)arg;
    for (int i = 0; i < obj->n; i++) {
        sem_wait(&obj->barSem); // Wait for permission to print bar
        printBar();
        sem_post(&obj->fooSem); // Signal foo() to print next
    }
    return NULL;
}

void fooBarFree(FooBar* obj) {
    sem_destroy(&obj->fooSem);
    sem_destroy(&obj->barSem);
    free(obj);
}

int main() {
    int n = 5; // Example: print "foobar" 5 times
    FooBar* obj = fooBarCreate(n);
    
    pthread_t threadA, threadB;

    // Create both threads
    pthread_create(&threadA, NULL, foo, obj);
    pthread_create(&threadB, NULL, bar, obj);

    // Wait for completion
    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);

    printf("\n");

    fooBarFree(obj);
    return 0;
}

/*
foobarfoobarfoobarfoobarfoobar
*/