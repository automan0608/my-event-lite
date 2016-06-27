
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "prefix_base.h"

int main()
{
    int result = 0;
    struct timeval tv1;
    struct timeval tv2;

    // 01
    {
        tv1.tv_sec = 200;
        tv1.tv_usec = 100;
 
        tv2.tv_sec = 100;
        tv2.tv_usec = 100;
 
        result = prefix_base_timeval_cmp(tv1, tv2);
        if (result == 1)
        {
            printf("test 01 success!\n");
        }
    }

    // 02
    {
        tv1.tv_sec = 100;
        tv1.tv_usec = 200;
 
        tv2.tv_sec = 100;
        tv2.tv_usec = 100;
 
        result = prefix_base_timeval_cmp(tv1, tv2);
        if (result == 1)
        {
            printf("test 02 success!\n");
        }
    }

    // 03
    {
        tv1.tv_sec = 100;
        tv1.tv_usec = 100;
 
        tv2.tv_sec = 100;
        tv2.tv_usec = 100;
 
        result = prefix_base_timeval_cmp(tv1, tv2);
        if (result == 0)
        {
            printf("test 03 success!\n");
        }
    }

    // 04
    {
        tv1.tv_sec = 100;
        tv1.tv_usec = 100;
 
        tv2.tv_sec = 200;
        tv2.tv_usec = 100;
 
        result = prefix_base_timeval_cmp(tv1, tv2);
        if (result == -1)
        {
            printf("test 04 success!\n");
        }
    }

    // 05
    {
        tv1.tv_sec = 100;
        tv1.tv_usec = 100;
 
        tv2.tv_sec = 100;
        tv2.tv_usec = 200;
 
        result = prefix_base_timeval_cmp(tv1, tv2);
        if (result == -1)
        {
            printf("test 05 success!\n");
        }
    }
}
