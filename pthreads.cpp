#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* run_mutex(void* value)
{
    pthread_mutex_t* p_mutex = (pthread_mutex_t*)value;
    pthread_mutex_lock(p_mutex);
    sleep(2);
    pthread_mutex_unlock(p_mutex);
    return NULL;
}

void* run_spinlock(void* value)
{
    pthread_spinlock_t* p_spinlock = (pthread_spinlock_t*)value;
    pthread_spin_lock(p_spinlock);
    sleep(2);
    pthread_spin_unlock(p_spinlock);
    return NULL;
}

void* run_read(void* value)
{
    pthread_rwlock_t* p_rdlock = (pthread_rwlock_t*)value;
    pthread_rwlock_rdlock(p_rdlock);
    sleep(2);
    pthread_rwlock_unlock(p_rdlock);
    return NULL;
}

void* run_write(void* value)
{
    pthread_rwlock_t* p_wrlock = (pthread_rwlock_t*)value;
    pthread_rwlock_wrlock(p_wrlock);
    sleep(2);
    pthread_rwlock_unlock(p_wrlock);
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
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_spinlock_t spinlock;
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);

    pthread_rwlock_t readlock = PTHREAD_RWLOCK_INITIALIZER;
    pthread_rwlock_t writelock = PTHREAD_RWLOCK_INITIALIZER;

    // lock lockers
    pthread_mutex_lock(&mutex);
    pthread_spin_lock(&spinlock);
    pthread_rwlock_rdlock(&readlock);
    pthread_rwlock_wrlock(&writelock);

    // creating threads
    pthread_t thread_mutex;
    pthread_create(&thread_mutex, NULL, run_mutex, (void*)&mutex);

    pthread_t thread_spinlock;
    pthread_create(&thread_spinlock, NULL, run_spinlock, (void*)&spinlock);

    pthread_t thread_read;
    pthread_create(&thread_read, NULL, run_read, (void*)&readlock);

    pthread_t thread_write;
    pthread_create(&thread_write, NULL, run_write, (void*)&writelock);

    // run
    sleep(20);

    // unlock lockers
    pthread_mutex_unlock(&mutex);
    pthread_spin_unlock(&spinlock);
    pthread_rwlock_unlock(&readlock);
    pthread_rwlock_unlock(&writelock);

    // join to threads
    pthread_join(thread_mutex, NULL);
    pthread_join(thread_spinlock, NULL);
    pthread_join(thread_read, NULL);
    pthread_join(thread_write, NULL);

    // destroy lockers
    pthread_spin_destroy(&spinlock);

    return 0;
}

