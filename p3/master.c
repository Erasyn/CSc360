#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <time.h>

void diskinfo(int argc, char* argv[]) {
    int ph = 1;
    //super block
    printf("Super block information:\nBlock size: %d\nBlock count: %d\nFAT starts: %d\n",ph,ph,ph);
    printf("FAT blocks: %d\nRoot directory start: %d\nRoot directory blocks %d\n\n",ph,ph,ph);
    // FAT info
    printf("FAT information:\nFree Blocks: %d\nReserved Blocks: %d\nAllocated Blocks: %d\n",ph,ph,ph);
    printf("%d\n",argv[1][0]);
    return;
}

void disklist(int argc, char* argv[]) {
    //for loop
    printf("c1 8d 20s 17s");
    return;
}

void diskget(int argc, char* argv[]) {
    return;
}

void diskput(int argc, char* argv[]) {
    return;
}


int main(int argc, char* argv[])
{
#if defined(PART1)
 diskinfo(argc, argv);
#elif defined(PART2)
 disklist(argc, argv);
#elif defined(PART3)
 diskget(argc, argv);
#elif defined(PART4)
 diskput(argc,argv);
#else
#error "PART[1234] must be defined"
#endif
 return 0;
}

// https://stackoverflow.com/questions/321423/parsing-binary-data-in-c