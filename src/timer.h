#ifndef __timer_h__
#define __timer_h__

#include <pthread.h>

typedef struct timer_vars_s timer_vars_t;

// BEGIN timer_vars_t
typedef struct timer_vars_s
{
    pthread_t thread_id;
    unsigned int cycle_time;
    void (*callback)(timer_vars_t* ptr_timer_vars);
} timer_vars_t;
// END timer_vars_t


void timer_start(timer_vars_t* ptr_timer_vars);
void timer_stop(timer_vars_t* ptr_timer_vars);
void* timer_thread(void*);
unsigned long long get_global_time();

#endif

