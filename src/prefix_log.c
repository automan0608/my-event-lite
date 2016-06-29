
#include "prefix_log.h"

#ifdef DEBUG
int loglevel = 4;
#endif

#ifdef INFO
int loglevel = 3;
#endif

#ifdef WARN
int loglevel = 2;
#endif

#ifdef ERROR
int loglevel = 1;
#endif

#ifndef DEBUG
#ifndef INFO
#ifndef WARN
#ifndef ERROR
int loglevel = 0;
#endif
#endif
#endif
#endif
