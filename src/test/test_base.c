
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "prefix_base.h"
#include "prefix_log.h"

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
            prefix_log("debug", "test 01 success!");
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
            prefix_log("debug", "test 02 success!");
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
            prefix_log("debug", "test 03 success!");
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
            prefix_log("debug", "test 04 success!");
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
            prefix_log("debug", "test 05 success!");
        }
    }
}
