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
struct dir_entry_timedate_t det;

void setDate(char* data, int offset, struct dir_entry_timedate_t* det) {
    uint32_t info = 0;
    memcpy(&info,data+offset,2);
    det->year = htons(info);
	memcpy(&det->month,data+offset+2,1);
	memcpy(&det->day,data+offset+3,1);
	memcpy(&det->hour,data+offset+4,1);
	memcpy(&det->minute,data+offset+5,1);
	memcpy(&det->second,data+offset+6,1);
    return;
}

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
		i < (sb.fat_block_count+sb.fat_start_block) * sb.block_size; i+=0x4) {
		memcpy(&info,address+i,8);
		info = htonl(info);
		if(info == 0x1) fi.res_blocks++; //reserve
		else if(info != 0x0) fi.alloc_blocks++; //allocate
		else fi.free_blocks++;
	}

    //printf("print img as string:%s(END)\n",address);
    
    //int temp=0x32;
    //memcpy(address+8,&temp,1);
    //printf("print img as string:%s(END)\n",address);

    munmap(address,buffer.st_size);
    close(fd);
}

void diskinfo(int argc, char* argv[]) {
	//initSuperBlock(argv);
	
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
	
	int fd = open(argv[1], O_RDWR);
    struct stat buffer;
	fstat(fd, &buffer);
	
    char* address=mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	
	//int tot_dir = (sb.block_size / 64) * sb.root_dir_block_count;
	struct dir_entry_t de;
	int info=0;
	uint32_t i;
	int start = sb.root_dir_start_block;
	int length = sb.root_dir_block_count;
	
	if(argc > 2) {
		//printf("more than %s\n",argv[2]);
		
		const char *my_str_literal = argv[2];
		char *token, *str, *tofree;

		tofree = str = strdup(my_str_literal);  // We own str's memory now.
		while ((token = strsep(&str, "/"))) {
			if (strlen(token) == 0) continue;
			//printf("%ld - %s\n",strlen(token),token);
			
			for(i = start * sb.block_size;
				i < (start+length) * sb.block_size; i+=64) {
				
				memcpy(&de.status,address+i,1);
				memcpy(&de.filename,address+i+27,31);
				if(strcmp((char*)de.filename,token) != 0 || de.status < 4) continue;
				//printf("match\n");
				
				memcpy(&info,address+i+1,4);
				start = htonl(info);
				memcpy(&info,address+i+5,4);
				length = htonl(info);
				memcpy(&info,address+i+9,4);
				de.size = htonl(info);
				//printf("size: %u\n",de.size);
				//printf("start: %d, length: %d\n",start,length);
				if(de.size > 0 && de.status == 5) {
					if(de.status == 2 || de.status == 3) de.status = 'F';
					else de.status = 'D';
				}
			}
		}
		free(tofree);
	}
	
	//printf("s:%d\n",start * sb.block_size);
	for(i = start * sb.block_size;
		i < (start+length) * sb.block_size; i+=64) {
			
		memcpy(&de.status,address+i,1);
		memcpy(&info,address+i+1,4);
		de.starting_block = htonl(info);
		memcpy(&info,address+i+5,4);
		de.block_count = htonl(info);
		memcpy(&info,address+i+9,4);
		de.size = htonl(info);
        setDate(address,i+13,&de.modify_time);
		setDate(address,i+20,&de.create_time);
		memcpy(&de.filename,address+i+27,31);
		memcpy(&de.unused,address+i+58,6);
		if(de.size > 0) {
			if(de.status == 2 || de.status == 3) de.status = 'F';
			else de.status = 'D';
			printf("%c ",de.status); // Fix this line
			printf("%10d ",de.size);
			printf("%30s ",de.filename);
            printf("%u/%02u/%02u %u:%02u:%02u\n",
                de.modify_time.year,de.modify_time.month,de.modify_time.day,
                de.modify_time.hour,de.modify_time.minute,de.modify_time.second);
		}
	}
	munmap(address,buffer.st_size);
    close(fd);

    return;
}

void diskget(int argc, char* argv[]) {
	
	int fd = open(argv[1], O_RDWR);
    struct stat buffer;
	fstat(fd, &buffer);
	
    char* address=mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	
	//int tot_dir = (sb.block_size / 64) * sb.root_dir_block_count;
	struct dir_entry_t de;
	int info=0;
	uint32_t i;
	int start = sb.root_dir_start_block;
	int length = sb.root_dir_block_count;
	int found = 0;
	
	
	if(argc != 4) {printf("wrong arg num\n"); return;}
	
	
	// nav to file
	if(argc > 2) {
		const char *my_str_literal = argv[2];
		char *token, *str, *tofree;
		tofree = str = strdup(my_str_literal);  // We own str's memory now.
		while ((token = strsep(&str, "/"))) {
			if (strlen(token) == 0) continue;
			
			for(i = start * sb.block_size;
				i < (start+length) * sb.block_size; i+=64) {
				
				memcpy(&de.status,address+i,1);
				memcpy(&de.filename,address+i+27,31);
				if(strcmp((char*)de.filename,token) != 0) continue;
				if(de.status == 3) found = 1;
				printf("match\n");
				
				memcpy(&info,address+i+1,4);
				start = htonl(info);
				memcpy(&info,address+i+5,4);
				length = htonl(info);
			}
			printf("start: %d\n",sb.fat_start_block * sb.block_size);
		}
		free(tofree);
	}
	printf("s: %d\n",start);
	
	if(!found) {printf("File not found.\n"); return;}
	
	uint32_t j;
	int fat = 0;
	printf("FAT: %d\n",(sb.fat_start_block * sb.block_size)+(start*4));
	char text[sb.block_size];
	FILE* fp = fopen(argv[3], "wb+");
	printf("new file\n");
	if(!fp) {printf("error\n"); return;}
	
	for(j = (sb.fat_start_block * sb.block_size)+(4*start);
		fat != -1; j=(sb.fat_start_block * sb.block_size)+(4*fat)) {
		int num = (j - (sb.fat_start_block * sb.block_size))/4*sb.block_size;
		
		memcpy(&fat,address+j,8);
		fat = htonl(fat);
		printf("info2: %d and i: %d\n",fat,num);
		
		
		memcpy(text,address+num, sb.block_size);
		//printf("%s\n",text);
		fwrite(text, sizeof(char), sb.block_size, fp);

	}
	fclose(fp);
	munmap(address,buffer.st_size);
    close(fd);
	
    return; //fprintf
}

void diskput(int argc, char* argv[]) {
    return;
}


int main(int argc, char* argv[]) {
	//mmap(), fread(), fwrite(), fseek(), fstat()
	if(argc < 2) {printf("img file required.\n"); return 0;}
	initSuperBlock(argv);
	
#if defined(PART1)
 diskinfo(argc, argv);
#elif defined(PART2)
 disklist(argc, argv);
#elif defined(PART3)
 diskget(argc, argv);
#elif defined(PART4)
 diskput(argc,argv);
//#else
//#error "PART[1234] must be defined"
#endif
 return 0;
}

// https://stackoverflow.com/questions/321423/parsing-binary-data-in-c
// https://stackoverflow.com/questions/26228515/how-do-you-store-2-hex-digits-1-byte-in-a-single-index-position-of-a-char-arra
// https://stackoverflow.com/questions/10324/convert-a-hexadecimal-string-to-an-integer-efficiently-in-c
// https://stackoverflow.com/questions/1394169/converting-hex-string-stored-as-chars-to-decimal-in-c