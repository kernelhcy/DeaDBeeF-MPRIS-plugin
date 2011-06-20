#ifndef __MPRIS_COMMON_H__
#define __MPRIS_COMMON_H__

#define MPRIS__DEBUG 1

#ifndef MPRIS__DEBUG
	#define debug(...)
#else
	#define debug(...)  do_debug(__VA_ARGS__)
#endif

void do_debug(const char *fmt, ...);


#endif
