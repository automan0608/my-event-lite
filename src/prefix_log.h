
#ifndef _PREFIX_LOG_H_
#define _PREFIX_LOG_H_

#include <stdio.h>

#if 1

#define prefix_log(level, fmt, arg...) do { 		\
		printf("%5s | %s:%s:%d | " fmt "\n", level, __FILE__, __FUNCTION__, __LINE__, ##arg); 	\
	} while (0)

#else

#define prefix_log(level, fmt) do {			\
		printf(level); printf(" | "); printf(fmt); printf("\n");		\
	} while (0)

#endif

#endif
