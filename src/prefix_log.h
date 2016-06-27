
#ifndef _PREFIX_LOG_H_
#define _PREFIX_LOG_H_

#include <stdio.h>


#define prefix_log(level, fmt, arg...) do { 		\
		printf("%5s | %s:%s:%d | " fmt "\n", level, __FILE__, __FUNCTION__, __LINE__, ##arg); 	\
	} while (0)


#endif
