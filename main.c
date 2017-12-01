#include <stdio.h>
#include <unistd.h>
#include "lunix.h"

void print_stats(stats);

int main(void) {
	int fd,i;
	long int size,r;
	char buf[100];
	char *tst="TESTING LUNIX DRIVER...";
	char *tst2="ABCDEFG";
	stats lstats;
	fd=open("/dev/lunix",1);
	if(fd<0) {
		perror("");
		exit(0);
	}
	
	ioctl(fd, LUNIX_GETSIZE, &size);
	printf("Old lunix size: %d\n", size);
	ioctl(fd, LUNIX_SETSIZE, 3000);
	ioctl(fd, LUNIX_GETSIZE, &size);
	printf("New lunix size: %d\n", size);	
	for(i=0;i<10;i++) write(fd,tst,strlen(tst) );
	lseek(fd, 0L, 0);
	write(fd,tst2,strlen(tst2) );
	close(fd);

	fd=open("/dev/lunix",0);
	read(fd,buf,100);
	buf[99]=0;
	puts(buf);
	ioctl(fd, LUNIX_GETSTATS, &lstats);
	close(fd);
	print_stats(lstats);
	return 0;
}

void print_stats(stats a) {
	printf("Lunix reads: %d\n", a.reads);
	printf("Lunix writes: %d\n", a.writes);
	printf("Lunix opens: %d\n", a.opens);
	printf("Lunix closes: %d\n", a.closes);
	printf("Lunix lseeks: %d\n", a.lseeks);
}

