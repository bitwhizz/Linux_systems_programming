
#include <pthread.h>
#include <stdio.h>

const char *NAME[] = {"foo", "bar", "baz", "blue", NULL};
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
  
// puting printf between the mutex lock/unlock
/*
we have a mutex protecting the counter++ operation but just above that line we access the counter value also printf is not thread safe,
to solve this we can just move the printf function to be inside the mutex protection.
*/
void *routine(void *data)
{
   static int counter;
  
   pthread_mutex_lock(&counter_mutex);
   printf("%s\n", NAME[counter]);
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


