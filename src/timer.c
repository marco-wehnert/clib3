#include "timer.h"
#include "utils.h"
#include <stddef.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// BEGIN timer_start
void timer_start(timer_vars_t* ptr_timer_vars)
{
    int result;
    result = pthread_create(&(ptr_timer_vars->thread_id), NULL, &timer_thread, ptr_timer_vars);
    if (result != 0) handle_error(result);
}
// END timer_start


void timer_stop(timer_vars_t* ptr_timer_vars)
{
    int result;
    void* ptr_thread_result;

    result = pthread_cancel(ptr_timer_vars->thread_id);
    if (result != 0) handle_error(result);

    result = pthread_join(ptr_timer_vars->thread_id, &ptr_thread_result);
    if (result != 0) handle_error(result);
}

void* timer_thread(void* ptr)
{
    unsigned long long last_time, current_time, diff_time;
    unsigned int cycle_time;
    timer_vars_t* ptr_timer_vars;

    ptr_timer_vars = (timer_vars_t*) ptr;
    cycle_time = ((timer_vars_t*) ptr)->cycle_time;
    last_time = get_global_time();

    while (1)
    {
        current_time = get_global_time();
        diff_time = current_time - last_time;
        if (diff_time >= cycle_time)
        {
            last_time += cycle_time;
            if (ptr_timer_vars->callback != NULL)
            {
                (*(ptr_timer_vars->callback))(ptr_timer_vars);
            }
        }
        usleep(1);
    }

    return (void*) 0;
}

unsigned long long get_global_time()
{
    int result;
    struct timeval now;
    unsigned long long time;
    result = gettimeofday(&now, NULL);
    if (result != 0)
    {
        perror(NULL);
        return 0;
    }
    time = now.tv_sec * 1000000 + now.tv_usec;
    return time;
}
