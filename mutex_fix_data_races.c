#include <pthread.h>
#include <stdio.h>

int FLAG = 0;

void *routine(void *data)
{
    int i = 0;
    pthread_mutex_t *flag_mutex;

    flag_mutex = (pthread_mutex_t *)data;
    while (1)
    {
        i = 1;
        pthread_mutex_lock(flag_mutex);
        if (FLAG == 1)
        {
           // do not forget to unlock a mutex after using it.
           pthread_mutex_unlock(flag_mutex);
           break ;
        }
        pthread_mutex_unlock(flag_mutex);
    }
    return (data);
}

int main(void)
{
    pthread_t new_thread;
 
    // creating a new mutex
    pthread_mutex_t flag_mutex;
    pthread_mutex_init(&flag_mutex, NULL);
    
    // since we didn't create the mutex in the global scope we need
    // to pass it to our routine function for the thread to be able
    // to access it.
    pthread_create(&new_thread, NULL, routine, (void *)&flag_mutex);

    int i = 0;
    while (i < 10)
    {
      if (i == 5)
      {
          pthread_mutex_lock(&flag_mutex);
          FLAG = 1;
          pthread_mutex_unlock(&flag_mutex);
      }
      i++;
    }

    pthread_join(new_thread, NULL);
    // destroying the mutex to free the resources
    pthread_mutex_destroy(&flag_mutex);
}