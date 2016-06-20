
#ifndef _PREFIX_LOG_H_
#define _PREFIX_LOG_H_

#define prefix_log(fmt, arg...) do { 		\
		printf(" %s | %s | " fmt "\n", __FUNCTION__, __LINE__, ##arg); 	\
	} while (0)

#endif
