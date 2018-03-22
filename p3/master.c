#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <time.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

// Super block
struct __attribute__((__packed__)) superblock_t {
 uint8_t fs_id [8];
 uint16_t block_size;
 uint32_t file_system_block_count;
 uint32_t fat_start_block;
 uint32_t fat_block_count;
 uint32_t root_dir_start_block;
 uint32_t root_dir_block_count;
};

// FAT info
struct fat_info_t {
 int free_blocks;
 int res_blocks;
 int alloc_blocks;
};

// Time and date entry
struct __attribute__((__packed__)) dir_entry_timedate_t {
 uint16_t year;
 uint8_t month;
 uint8_t day;
 uint8_t hour;
 uint8_t minute;
 uint8_t second;
};

// Directory entry
struct __attribute__((__packed__)) dir_entry_t {
 uint8_t status;
 uint32_t starting_block;
 uint32_t block_count;
 uint32_t size;
 struct dir_entry_timedate_t modify_time;
 struct dir_entry_timedate_t create_time;
 uint8_t filename[31];
 uint8_t unused[6];
};

struct superblock_t sb;
struct fat_info_t fi;

void initSuperBlock(char* argv[]) {
		
	int fd = open(argv[1], O_RDWR);
    struct stat buffer;
    //int status = 
	fstat(fd, &buffer);
	//printf("stat: %d\n",(int)buffer.st_blocks);

    //tamplate:   pa=mmap(addr, len, prot, flags, fildes, off);
    //c will implicitly cast void* to char*, while c++ does NOT
    char* address=mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    uint32_t info=0;
	memcpy(&sb.fs_id,address,8);
	//printf("%s\n",sb.fs_id);
    memcpy(&info,address+8,2);
    sb.block_size=htons(info);
	memcpy(&info,address+10,4);
    sb.file_system_block_count=htonl(info);
	memcpy(&info,address+14,4);
    sb.fat_start_block=htonl(info);
	memcpy(&info,address+18,4);
    sb.fat_block_count=htonl(info);
	memcpy(&info,address+22,4);
    sb.root_dir_start_block=htonl(info);
	memcpy(&info,address+26,4);
    sb.root_dir_block_count=htonl(info);
	
	
	//char temp[512];
	
	//memcpy(&info,address+0x4d4,8);
    //info=htonl(info);
	//printf("info:\n%d\n",htonl(info));
	
	//uint32_t info_t;
	
	long i;
	fi.free_blocks = 0;
	fi.res_blocks = 0;
	fi.alloc_blocks = 0;
	//printf("FAT: %ld\n",buffer.st_size);
	for(i = sb.fat_start_block * sb.block_size;
		i < sb.fat_block_count * sb.block_size; i+=0x4) {
		memcpy(&info,address+i,8);
		info = htonl(info);
		if(info == 0x1) fi.res_blocks++; //reserve
		else if(info != 0x0) fi.alloc_blocks++; //allocate
	} fi.free_blocks = sb.file_system_block_count - fi.alloc_blocks - fi.res_blocks;

    //printf("print img as string:%s(END)\n",address);
    
    //int temp=0x32;
    //memcpy(address+8,&temp,1);
    //printf("print img as string:%s(END)\n",address);

    munmap(address,buffer.st_size);
    close(fd);
	
	
	/*
    fseek(in, 0x8, SEEK_SET); // go to partition table start
    fread(sb.block_size, 1, 2, in);
	fread(&buffer, 1, 4, in);
	//sb.block_count = strtol(buffer,NULL,16);
	printf("%ld this\n",strtol(&buffer,NULL,16));
	fread(sb.fat_starts, 1, 4, in);
	fread(sb.fat_blocks, 1, 4, in);
	fread(sb.root_start, 1, 4, in);
	fread(sb.root_blocks, 1, 4, in);
    
	
	int i;
	for(i = 0; i < 8; i+=2)
		printf("%.2X%.2X ", buffer[i],buffer[i+1]);
	printf("\n");
	fread(buffer, 1, 8, in);
	for(i = 0; i < 8; i+=2)
		printf("%.2X%.2X ", buffer[i],buffer[i+1]);
	printf("\n");
	*/
    
    //fclose(in);
	
}

void diskinfo(int argc, char* argv[]) {
    //int ph = 1;
	
	initSuperBlock(argv);
	//(unsigned char)sb.block_size[0] * 256 + (unsigned char)sb.block_size[1]
	
    //super block
    printf("Super block information:\nBlock size: %d\nBlock count: %d\nFAT starts: %d\n",
		sb.block_size, sb.file_system_block_count, sb.fat_start_block);
    printf("FAT blocks: %d\nRoot directory start: %d\nRoot directory blocks %d\n\n",
		sb.fat_block_count,sb.root_dir_start_block,sb.root_dir_block_count);
    // FAT info
    printf("FAT information:\nFree Blocks: %d\nReserved Blocks: %d\nAllocated Blocks: %d\n",
		fi.free_blocks,fi.res_blocks,fi.alloc_blocks);
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
// https://stackoverflow.com/questions/10324/convert-a-hexadecimal-string-to-an-integer-efficiently-in-c
// https://stackoverflow.com/questions/1394169/converting-hex-string-stored-as-chars-to-decimal-in-c