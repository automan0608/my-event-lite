
#include <stdlib.h>
#include <stdio.h>

#include "event.h"

pthread_t tid;

void cb01(int fd, short events, void *arg)
{
    printf("In callback 01\n");
}

void cb02(int fd, short events, void *arg)
{
    printf("In callback 02\n");
}

void cb03(int fd, short events, void *arg)
{
    printf("In callback 03\n");
}

void *thread_fn(void *arg)
{
    prefix_event_base_t *base = (prefix_event_base_t *)arg;
    printf("in thread fn\n");
    prefix_event_base_dispatch(base);
}

int main()
{
    prefix_event_base_t *base = prefix_event_base_new();
    prefix_event_base_use_thread(base);

    struct timeval tv01 = {1, 0};
    prefix_event_t *event01 = prefix_event_new(base, 0, EV_TIME|EV_PERSIST, &tv01, cb01, NULL);
//    prefix_event_t *event01 = prefix_event_new(base, 0, EV_TIME, &tv01, cb01, NULL);

    int result = pthread_create(&tid, NULL, thread_fn, (void *)base);
    if (0 != result)
    {
        printf("thread create error\n");
        return -1;
    }

    sleep(3);

    struct timeval tv02 = {2, 500000};
    prefix_event_t *event02 = prefix_event_new(base, 0, EV_TIME|EV_PERSIST, &tv02, cb02, NULL);
//    prefix_event_t *event02 = prefix_event_new(base, 0, EV_TIME, &tv02, cb02, NULL);

    sleep(3);

    struct timeval tv03 = {3, 300000};
//    prefix_event_t *event03 = prefix_event_new(base, 0, EV_TIME, &tv03, cb03, NULL);
    prefix_event_t *event03 = prefix_event_new(base, 0, EV_TIME|EV_PERSIST, &tv03, cb03, NULL);

    while(1)
        pause();
}
