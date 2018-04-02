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

void putDate(char* data, int offset) {
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	timeinfo->tm_year += 1900;
	timeinfo->tm_mon += 1;


    uint32_t info = 0;
	info = htons(timeinfo->tm_year);
    memcpy(data+offset,&info,2);
	memcpy(data+offset+2,&timeinfo->tm_mon,1);
	memcpy(data+offset+3,&timeinfo->tm_mday,1);
	memcpy(data+offset+4,&timeinfo->tm_hour,1);
	memcpy(data+offset+5,&timeinfo->tm_min,1);
	memcpy(data+offset+6,&timeinfo->tm_sec,1);
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
	int found = 0;
	
	// nav to file
	if(argc > 2) {
		const char *my_str_literal = argv[2];
		char *token, *str, *tofree;
		tofree = str = strdup(my_str_literal);  // We own str's memory now.
		while ((token = strsep(&str, "/"))) {
			if (strlen(token) == 0) continue;
			// This means a directory was given

			uint32_t j;
			int fat = 0;
			//printf("FAT: %d\n",(sb.fat_start_block * sb.block_size)+(start*4));
			
			for(j = (sb.fat_start_block * sb.block_size)+(4*start); // goes to root in fat
				fat != -1; j=(sb.fat_start_block * sb.block_size)+(4*fat)) {
				// actual location
				int num = (j - (sb.fat_start_block * sb.block_size))/4*sb.block_size;
				// get next loc in fat
				memcpy(&fat,address+j,8);
				fat = htonl(fat);
				//printf("info2: %d and i: %d\n",fat,num);
				//printf("%d\n",num);

				//parse curr dir block
				for(i = num; i < num + sb.block_size; i+=64) {

					memcpy(&de.status,address+i,1);
					memcpy(&de.filename,address+i+27,31);
					// no match or not dir
					if(strcmp((char*)de.filename,token) != 0 || de.status != 5) continue;
					// getting here means we're in at least 1 dir
					found = 1;
					//printf("match\n");
					
					memcpy(&start,address+i+1,4);
					start = htonl(start);
				}
			}
			//printf("start: %d\n",sb.fat_start_block * sb.block_size);
		}
		free(tofree);
	}
	//printf("s: %d\n",start);
	
	if(!found) {start = sb.root_dir_start_block;//printf("not found listing root\n");}
	}
	//printf("s: %d\n",start);
	
	uint32_t j;
	int fat = 0;
	//printf("FAT: %d\n",(sb.fat_start_block * sb.block_size)+(start*4));
	
	for(j = (sb.fat_start_block * sb.block_size)+(4*start);
		fat != -1; j=(sb.fat_start_block * sb.block_size)+(4*fat)) {
		int num = (j - (sb.fat_start_block * sb.block_size))/4*sb.block_size;
		memcpy(&fat,address+j,8);
		fat = htonl(fat);
		//printf("info2: %d and i: %d\n",fat,num);
		//printf("%d\n",num);
		
		for(i = num; i < num + sb.block_size; i+=64) {
				
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
				printf("%c ",de.status);
				printf("%10d ",de.size);
				printf("%30s ",de.filename);
				printf("%u/%02u/%02u %u:%02u:%02u\n",
					de.modify_time.year,de.modify_time.month,de.modify_time.day,
					de.modify_time.hour,de.modify_time.minute,de.modify_time.second);
			}
		}

	}
	munmap(address,buffer.st_size);
    close(fd);

    return;
}

void diskget(int argc, char* argv[]) {  // fix dir jumping
	
	int fd = open(argv[1], O_RDWR);
    struct stat buffer;
	fstat(fd, &buffer);
	
    char* address=mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	
	//int tot_dir = (sb.block_size / 64) * sb.root_dir_block_count;
	struct dir_entry_t de;
	int info=0;
	uint32_t i;
	uint32_t size;
	int start = sb.root_dir_start_block;
	int found = 0;
	
	
	if(argc != 4) {printf("./diskget test.img <file to get> <name on local system>\n"); return;}
	
	
	// nav to file
	if(argc > 2) {
		const char *my_str_literal = argv[2];
		char *token, *str, *tofree;
		tofree = str = strdup(my_str_literal);  // We own str's memory now.
		while ((token = strsep(&str, "/"))) {
			if (strlen(token) == 0) continue;
			// This means a directory was given

			uint32_t j;
			int fat = 0;
			//printf("FAT: %d\n",(sb.fat_start_block * sb.block_size)+(start*4));
			
			for(j = (sb.fat_start_block * sb.block_size)+(4*start); // goes to root in fat
				fat != -1; j=(sb.fat_start_block * sb.block_size)+(4*fat)) {
				// actual location
				int num = (j - (sb.fat_start_block * sb.block_size))/4*sb.block_size;
				// get next loc in fat
				memcpy(&fat,address+j,8);
				fat = htonl(fat);
				//printf("info2: %d and i: %d\n",fat,num);
				//printf("%d\n",num);

				//parse curr dir block
				for(i = num; i < num + sb.block_size; i+=64) {

					memcpy(&de.status,address+i,1);
					memcpy(&de.filename,address+i+27,31);
					//printf("omg %s\n",de.filename);
					// checks if is a file or dir and match
					if(strcmp((char*)de.filename,token) != 0) {
						//if(de.status != 0) found = 0;
						continue;
					}
					if(de.status == 3) found = 1;
					//printf("match\n");
					
					memcpy(&info,address+i+1,4);
					start = htonl(info);
					memcpy(&info,address+i+9,4);
					size = htonl(info);
					//printf("the s: %d\n",size);
					//break;
				}
				//if(found) break;
			}
			//printf("start: %d\n",start * sb.block_size);
		}
		free(tofree);
	}
	//printf("s: %d\n",start);
	
	if(!found) {printf("File not found.\n"); return;}
	
	uint32_t j;
	int fat = 0;
	//printf("FAT: %d\n",(sb.fat_start_block * sb.block_size)+(start*4));
	char text[sb.block_size];
	FILE* fp = fopen(argv[3], "wb+");
	//printf("new file\n");
	if(!fp) {printf("error\n"); return;}
	// keep track of file size i guess
	for(j = (sb.fat_start_block * sb.block_size)+(4*start);
		fat != -1; j=(sb.fat_start_block * sb.block_size)+(4*fat)) {
		int num = (j - (sb.fat_start_block * sb.block_size))/4*sb.block_size;
		
		memcpy(&fat,address+j,8);
		fat = htonl(fat);
		//printf("info2: %d and i: %d\n",fat,num);
		
		//Fix the garbage with the img file get. wrong size, too big. doesnt break
		memcpy(text,address+num, sb.block_size);
		//printf("%s\n",text);
		//this handles writing the end of file properly
		if(size < sb.block_size) fwrite(text, sizeof(char), size, fp);
		else fwrite(text, sizeof(char), sb.block_size, fp);
		//printf("it size: %u\n",size);
		if(size - sb.block_size > size) break;
		size -= sb.block_size;

	}
	fclose(fp);
	munmap(address,buffer.st_size);
    close(fd);
	
    return;
}

void toUint(char* in, uint8_t* cnv) {
	int i = 0;
	memset(cnv,0,31);
	//uint8_t cnv[31];
	for(i = 0; i < strlen(in); i++)
		cnv[i] = in[i];
}

void diskput(int argc, char* argv[]) {
	
	if(argc < 3) {printf("./diskput test.img <file to place> <optional name for the file and subdirs>\n"); return;}
	
	int fd = open(argv[1], O_RDWR);
	struct stat buffer;
	fstat(fd, &buffer);
	
    char* address=mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	
	uint8_t buf[sb.block_size];
	FILE* fp = fopen(argv[2], "rb+");
	if(!fp) {printf("File not found.\n"); return;}
	struct stat st;
	stat(argv[2],&st);
	int size = st.st_size;
	//if(size > fi.free_blocks * sb.block_size) {printf("File too large\n");return;}
	
	struct dir_entry_t de;
	long info=0;
	uint32_t i;
	int start = sb.root_dir_start_block;
	int found = 1;
	char* filename = 0;
	
	
	// nav to dest folder else stay in root
	if(argc > 3) {
		//found = 0;
		const char *my_str_literal = argv[3];
		char *token, *str, *tofree;
		tofree = str = strdup(my_str_literal);  // We own str's memory now.
		while ((token = strsep(&str, "/"))) {
			if (strlen(token) == 0) continue;
			// This means a directory was given
			found = 0;
			uint32_t j;
			int fat = 0;
			//printf("FAT: %d\n",(sb.fat_start_block * sb.block_size)+(start*4));
			
			for(j = (sb.fat_start_block * sb.block_size)+(4*start); // goes to root in fat
				fat != -1; j=(sb.fat_start_block * sb.block_size)+(4*fat)) {
				// actual location
				int num = (j - (sb.fat_start_block * sb.block_size))/4*sb.block_size;
				// get next loc in fat
				memcpy(&fat,address+j,8);
				fat = htonl(fat);
				//printf("info2: %d and i: %d\n",fat,num);
				//printf("%d\n",num);

				//parse curr dir block
				for(i = num; i < num + sb.block_size; i+=64) {

					memcpy(&de.status,address+i,1);
					memcpy(&de.filename,address+i+27,31);
					//printf("token: %s\n",token);
					// no match or not dir
					if(de.status == 2 || de.status == 3) filename = (char*)de.filename;
					else if(de.status != 5) {filename = strdup(token); found = 1;}
					if(strcmp((char*)de.filename,token) != 0 || de.status != 5) continue;
					// getting here means we're in at least 1 dir
					found = 1;
					filename = argv[2];
					//printf("match\n");
					
					memcpy(&start,address+i+1,4);
					start = htonl(start);
				}
			}
			//printf("start: %d\n",sb.fat_start_block * sb.block_size);
		}
		free(tofree);
	} else filename = argv[2];
	if(!found) {printf("Directory not found.\n"); return;}
	
	//find first empty FAT table spot
	// make fat parsing inside loop, will have to keep finding empties.
	long j,k;
	uint32_t fat_entry = 0;
	uint8_t xf[31] = {0};
	memset(&buf,0,sizeof(buf));
	long f = fread(buf,sizeof(char),sizeof(buf),fp);
	int fat = 0;
	int zero = -1;
	// this parses fat consts fine
	for(k = sb.fat_start_block * sb.block_size;
		(k < (sb.fat_block_count+sb.fat_start_block) * sb.block_size); k+=0x4) {
		// check curr byte
		memcpy(&info,address+k,8);
		info = htonl(info);
		//find empty fat block
		if(info == 0x0) {
			// for first one we have to put stats in its dir
			if(fat_entry == 0) {

				// this where we working
				// j is the location of our dir in fat
				// this will parse the curr dir fat entries
				for(j = (sb.fat_start_block * sb.block_size)+(4*start);
					fat != -1; j=(sb.fat_start_block * sb.block_size)+(4*fat)) {
					// actual location
					//printf("this j: %ld\n",j);
					int num = (j - (sb.fat_start_block * sb.block_size))/4*sb.block_size;
					
					// get next loc in fat
					memcpy(&fat,address+j,8);
					fat = htonl(fat);
					//printf("infosec: %d and i: %d\n",fat,num);
					//printf("%d\n",num);

					//parse curr dir block
					
					for(i = num; i < num + sb.block_size; i+=64) {

						memcpy(&de.status,address+i,1);
						memcpy(&de.filename,address+i+27,31);
						memcpy(&de.starting_block,address+i+1,4);
						de.starting_block = htonl(de.starting_block);
						// if has dest folder
						//printf("filenames: %s\n and fname: %s\n",de.filename,filename);
						if(de.status % 2 == 0 && zero == -1) {zero = i;}//printf("z: %d\n",zero);}
						if(strcmp((char*)de.filename,filename) != 0) {
							continue; 
						}
						// if just insert, look for empty spot.
						// getting here means we're good to do stuff
						//printf("match\n");
						
						long z = 0;
						int ft = 0;
						int zr = 0x0;
						int sv;
						//I guess at this point we would parse and clear this files fat entries.
						for(z = (sb.fat_start_block * sb.block_size) + (4*de.starting_block);
							sv != -1; z=(sb.fat_start_block * sb.block_size)+(4*sv)) {
							//printf("zed: %ld\n",(de.starting_block));
							memcpy(&ft,address+z,4);
							sv = htonl(ft);
							//printf("overwrite: %d\n",sv);
							memcpy(address+z,&zr,4);
							

						}
						zero = (sb.fat_start_block * sb.block_size) + (4*de.starting_block);
						k = zero;
						memcpy(address+1,&zr,1);


						//printf("a name: %s\n an i: %d\n",filename,i);

						// info numbers are all 
						info = 3;
						memcpy(address+i,&info,1);
						info = htonl((k - (sb.fat_start_block * sb.block_size))/4); //start block
						memcpy(address+i+1,&info,4);
						if(size % sb.block_size == 0) info = size / sb.block_size;
						else info = (size / sb.block_size)+1;
						info = htonl(info);
						memcpy(address+i+5,&info,4);
						size = htonl(size);
						memcpy(address+i+9,&size,4);
						toUint(filename,xf);
						memcpy(address+i+27,xf,31);
						// fine to here for info
						// TODO dates

						putDate(address,i+13);
						putDate(address,i+20);

						info = 0xFFFFFFFFFFFF;
						memcpy(address+i+58,&info,6);
						filename = "/";
					}
					//file doesnt yet exist
					if(strcmp(filename,"/") != 0) {
						// if just insert, look for empty spot.
						//else if (de.status != 0) continue;
						// getting here means we're good to do stuff
						//printf("match final\n");
						//filename = "testname.txt";
						//filename = argv[2];
						

						//printf("a name: %s\n",filename);

						// info numbers are all 
						info = 3;
						memcpy(address+zero,&info,1);
						info = htonl((k - (sb.fat_start_block * sb.block_size))/4); //start block
						memcpy(address+zero+1,&info,4);
						if(size % sb.block_size == 0) info = size / sb.block_size;
						else info = (size / sb.block_size)+1;
						info = htonl(info);
						memcpy(address+zero+5,&info,4);
						size = htonl(size);
						memcpy(address+zero+9,&size,4);
						toUint(filename,xf);
						memcpy(address+zero+27,xf,31);
						// fine to here for info

						putDate(address,zero+13);
						putDate(address,zero+20);

						info = 0xFFFFFFFFFFFF;
						memcpy(address+i+58,&info,6);
						filename = "/";
					}
				}
			}

			// down here no touch
			// but fat entries are probably wrong.

			fat_entry = (k - (sb.fat_start_block * sb.block_size))/4;
			//printf("empty: %d\n",fat_entry);

			memcpy(address+(fat_entry*sb.block_size),&buf,j);
			//printf("print img as string:%s(END)\n",address+(fat_entry*sb.block_size));
			memset(&buf,0,sizeof(buf));
			// That was the last read
			if((f = fread(buf,sizeof(char),sizeof(buf),fp)) == 0) {
				unsigned int loc = 0xFFFFFFFF;
				memcpy(address+(sb.fat_start_block*sb.block_size)+(fat_entry*4),&loc,4);
				break;
			} else {
				int loc = fat_entry+1;//(k - (sb.fat_start_block * sb.block_size))/4;
				loc = htonl(loc);
				memcpy(address+(sb.fat_start_block*sb.block_size)+(fat_entry*4),&loc,4);
			}
		}
	}
	fclose(fp);
	munmap(address,buffer.st_size);
    close(fd);
	
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