#ifndef _COMMON_H_
#define _COMMON_H_


#include <stdio.h>
#include <stdlib.h>

#define MAJOR 1
#define MINOR 0
#define PATCH 0 
#define RELEASE 0
#define DATE	__DATE__
#define TIME  __TIME__
#define VERSION() (RELEASE | (PATCH << 8) | (MINOR << 16) | (MAJOR << 24))
#define VERSION_STR() ("V"##MAJOR##"."##MINOR##"."##PATCH##"."##RELEASE)

#define MALLOC(size) malloc(size)
#define FREE(p) free(p)

#ifndef bool
#define bool unsigned int
#endif

#ifndef true
#define true (!!1)
#endif

#ifndef false
#define false (!!0)
#endif

#ifndef u64
#define u64 unsigned long long
#endif

#ifndef s64
#define s64 long long
#endif

//#define ASSERT(x) do { if (!(x)) exit(0); } while (0)

#define ASSERT(x)	do { if (!(x)) { printf("Assert Error [%s->%d]!\n", __func__, __LINE__);exit(0); } } while (0);
#endif
