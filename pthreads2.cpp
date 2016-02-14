#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

struct cond_mutex
{
    pthread_mutex_t* p_mutex;
    pthread_cond_t* p_cond;
};

void* run_cond(void* value)
{
    cond_mutex* p_cond_mutex = (cond_mutex*)value;
    pthread_cond_wait(p_cond_mutex->p_cond, p_cond_mutex->p_mutex);
    return NULL;
}

void* run_barrier(void* value)
{
    pthread_barrier_t* p_barrier = (pthread_barrier_t*)value;
    pthread_barrier_wait(p_barrier);
    return NULL;
}


int main(int argc, char** argv)
{/*
    FILE* f = fopen("/home/box/main.pid", "w");
    if(f)
    {
        fprintf(f, "%d\n", getpid());
        fclose(f);
    }*/

    // creating lockers
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    struct cond_mutex cm;
    cm.p_cond = &cond;
    cm.p_mutex = &mutex;

    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, 2);

    // creating threads
    pthread_t thread_cond;
    pthread_create(&thread_cond, NULL, run_cond, (void*)&cm);

    pthread_t thread_barrier;
    pthread_create(&thread_barrier, NULL, run_barrier, (void*)&barrier);

    // run
    sleep(20);

    // free lockers
    pthread_cond_broadcast(&cond);
    pthread_barrier_wait(&barrier);

    // join to threads
    pthread_join(thread_cond, NULL);
    pthread_join(thread_barrier, NULL);

    // destroy lockers
    pthread_cond_destroy(&cond);
    pthread_barrier_destroy(&barrier);

    return 0;
}

