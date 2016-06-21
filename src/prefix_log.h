
#ifndef _PREFIX_LOG_H_
#define _PREFIX_LOG_H_

#include <stdio.h>

#define prefix_log(level, fmt, arg...) do { 		\
		printf(level " | %s | %s | " fmt "\n", __FUNCTION__, __LINE__, ##arg); 	\
	} while (0)

#endif
