#include <linux/ioctl.h>

#define LUNIX_MAJOR 250

typedef struct _stats {
	int opens;
	int closes;
	int reads;
	int writes;
	int lseeks;
} stats;

#define LUNIX_INIT _IO(LUNIX_MAJOR,0)
#define LUNIX_GETSIZE _IOR(LUNIX_MAJOR,1,int)
#define LUNIX_SETSIZE _IOW(LUNIX_MAJOR,2,int)
#define LUNIX_GETSTATS _IOR(LUNIX_MAJOR,3,stats)

