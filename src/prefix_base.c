
#include "prefix_base.h"

int prefix_base_timeval_cmp(const struct timeval tv1, const struct timeval tv2)
{
	if (tv1.tv_sec > tv2.tv_sec)
	{
		return 1;
	}
	else if (tv1.tv_sec == tv2.tv_sec)
	{
		if (tv1.tv_usec > tv2.tv_usec)
		{
			return 1;
		}
		else if (tv1.tv_usec == tv2.tv_usec)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}
