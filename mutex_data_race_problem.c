#include <pthread.h>
#include <stdio.h>

const char *NAME[] = {"foo", "bar", "baz", "blue", NULL};
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
  
  /*
  If you need to lock multiple mutexes to protect multiple data you have to follow a certain order:

LOCK MUTEX1
LOCK MUTEX2
...
UNLOCK MUTEX2
UNLOCK MUTEX1
the inner mutexes have to be unlocked first. 
It’s also very important that you always follow the same order of lock/unlock across multiple threads 
because otherwise it can lead to deadlocks due to lock inversion for example:

T1:
LOCK MUTEX1
LOCK MUTEX2
...
UNLOCK MUTEX2
UNLOCK MUTEX1

T2:
LOCK MUTEX2
LOCK MUTEX1
...
UNLOCK MUTEX1
UNLOCK MUTEX2
In the situation above if both threads acquire 1 mutex the program will be deadlocked 
since T1 will be waiting on MUTEX2 hold by T2 and T2 will be waiting for MUTEX1 hold by T1. 
So always follow the same order across multiple threads.
  
  */
  
void *routine(void *data)
{
   static int counter;
  
   printf("%s\n", NAME[counter]);
   pthread_mutex_lock(&counter_mutex);
   counter++;
   pthread_mutex_unlock(&counter_mutex);
   return (NULL);
}

int main(void)
{
   int   i = -1;
   pthread_t threads[4];
  
   while (++i < 4)
      pthread_create(&threads[i], NULL, routine, NULL);
   while (--i >= 0)
      pthread_join(threads[i], NULL);
}


/*

Tools for diagnosing thread problems
-fsanitize=thread (compiler flag)
valgrind --tool=helgrind
valgrind --tool=drd


saif@saif-Inspiron-5547:~/programmer/Linux_OS_Concepts$ gcc mutex_data_race_problem.c -o mutex -fsanitize=thread -lpthread
saif@saif-Inspiron-5547:~/programmer/Linux_OS_Concepts$ ./mutex 
foo
==================
WARNING: ThreadSanitizer: data race (pid=9004)
  Read of size 4 at 0x000000601128 by thread T2:
    #0 routine <null> (mutex+0x4009f4)
    #1 <null> <null> (libtsan.so.0+0x2abf6)

  Previous write of size 4 at 0x000000601128 by thread T1 (mutexes: write M0):
    #0 routine <null> (mutex+0x400a49)
    #1 <null> <null> (libtsan.so.0+0x2abf6)

  Location is global 'counter.3384' of size 4 at 0x000000601128 (mutex+0x000000601128)

  Mutex M0 (0x000000601100) created at:
    #0 pthread_mutex_lock <null> (libtsan.so.0+0x41dab)
    #1 routine <null> (mutex+0x400a2c)
    #2 <null> <null> (libtsan.so.0+0x2abf6)

  Thread T2 (tid=9007, running) created by main thread at:
    #0 pthread_create <null> (libtsan.so.0+0x2d40e)
    #1 main <null> (mutex+0x400ac2)

  Thread T1 (tid=9006, finished) created by main thread at:
    #0 pthread_create <null> (libtsan.so.0+0x2d40e)
    #1 main <null> (mutex+0x400ac2)

SUMMARY: ThreadSanitizer: data race (/home/saif/programmer/Linux_OS_Concepts/mutex+0x4009f4) in routine
==================
bar
baz
blue
ThreadSanitizer: reported 1 warnings
*/