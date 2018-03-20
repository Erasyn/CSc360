#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <time.h>
#include <math.h>

typedef struct {
    unsigned char block_size[2];
    unsigned char block_count[4];
    unsigned char fat_starts[4];
    unsigned char fat_blocks[4];
    unsigned char root_start[4];
    unsigned char root_blocks[4];
	unsigned char free_blocks[1];
	unsigned char rsrv_blocks[1];
	unsigned char alloc_blocks[1];
} SuperBlock;

SuperBlock sb;
/*
int getVal(unsigned char* str) {
	int len = sizeof(str);
	int i;
	for(i = 0; i < len; i++) {
		int mp = (l1
	}
	
}*/

void initSuperBlock(char* argv[]) {
		
	FILE* in = fopen(argv[1], "rb");
	unsigned int buffer;
    
    fseek(in, 0x8, SEEK_SET); // go to partition table start
    fread(sb.block_size, 1, 2, in);
	fread(&buffer, 1, 4, in);
	//sb.block_count = strtol(buffer,NULL,16);
	printf("%ld this\n",strtol(&buffer,NULL,16));
	fread(sb.fat_starts, 1, 4, in);
	fread(sb.fat_blocks, 1, 4, in);
	fread(sb.root_start, 1, 4, in);
	fread(sb.root_blocks, 1, 4, in);
    
	/*
	int i;
	for(i = 0; i < 8; i+=2)
		printf("%.2X%.2X ", buffer[i],buffer[i+1]);
	printf("\n");
	fread(buffer, 1, 8, in);
	for(i = 0; i < 8; i+=2)
		printf("%.2X%.2X ", buffer[i],buffer[i+1]);
	printf("\n");
	*/
    
    fclose(in);
	
}

void diskinfo(int argc, char* argv[]) {
    int ph = 1;
	
	initSuperBlock(argv);
	//(unsigned char)sb.block_size[0] * 256 + (unsigned char)sb.block_size[1]
	
    //super block
//    printf("Super block information:\nBlock size: %d\nBlock count: %d\nFAT starts: %s\n",
//		(unsigned char)sb.block_size[0] * 256 + (unsigned char)sb.block_size[1],
//		sb.block_count,sb.fat_starts);
    printf("FAT blocks: %d\nRoot directory start: %d\nRoot directory blocks %d\n\n",
		ph,ph,ph);
    // FAT info
    printf("FAT information:\nFree Blocks: %d\nReserved Blocks: %d\nAllocated Blocks: %d\n",
		ph,ph,ph);
    printf("%d\n",argv[1][2]);
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


int main(int argc, char* argv[]) {
	//mmap(), fread(), fwrite(), fseek(), fstat()
	
#if defined(PART1)
 diskinfo(argc, argv);/*
#elif defined(PART2)
 disklist(argc, argv);
#elif defined(PART3)
 diskget(argc, argv);
#elif defined(PART4)
 diskput(argc,argv);
#else
#error "PART[1234] must be defined"*/
#endif
 return 0;
}

// https://stackoverflow.com/questions/321423/parsing-binary-data-in-c
// https://stackoverflow.com/questions/26228515/how-do-you-store-2-hex-digits-1-byte-in-a-single-index-position-of-a-char-arra