
#ifndef _PREFIX_LOG_H_
#define _PREFIX_LOG_H_

#include <stdio.h>
#include <string.h>
#include <time.h>

extern int loglevel;
extern char timebuf[];

#define prefix_log_raw(level, fmt, arg...) do { 				\
		time_t timer = time(NULL); 								\
		strftime(timebuf, 20, "%Y-%m-%d %H:%M:%S", localtime(&timer)); 	\
		printf("%s | %5s | %20s : %25s : %d | " fmt "\n", timebuf, level, __FILE__, __FUNCTION__, __LINE__, ##arg); 	\
	} while (0)

#define prefix_log(level, fmt, arg...) do { 		\
		if (!strcmp(level, "debug")) 				\
			if (4 <= loglevel)						\
				prefix_log_raw(level, fmt, ##arg);	\
		if (!strcmp(level, "info")) 				\
			if (3 <= loglevel)						\
				prefix_log_raw(level, fmt, ##arg);	\
		if (!strcmp(level, "warn")) 				\
			if (3 <= loglevel)						\
				prefix_log_raw(level, fmt, ##arg);	\
		if (!strcmp(level, "error")) 				\
			if (3 <= loglevel)						\
				prefix_log_raw(level, fmt, ##arg);	\
	} while (0)

#endif
