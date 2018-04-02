Subdirectories are implemented for Part 2 and 3 definitely. 4 does work, but some edge cases may exist.

PART 1: 
parse the super block and assign values into struct. print these values

PART 2: 
find the root dir in FAT table, then loop through its blocks based on FAT
entries until all blocks processed. For subdirs, do the same after iterating through previous
dirs in FAT to find the relevant subdir.

PART 3: 
Similar process to part 2 for finding a subdir if given. Will then loop through dir for specific filename, and then proceed to fwrite to a file and save locally. Keeps track of size of
file to maintain same size over transfer.

PART 4: 
Same as previous parts for subdir processing. Won't expand a directory.
If you write a file that already exists it will overwrite it and modify the FAT blocks accordingly,
as it will first check it a specified filename is in the .img file.

SUBDIRS:
Overall, subdirectories work in this program, but in part 4 the naming of file can be a little buggy. They will still be input to the file system though.

Doing it like /diskput test.img <file to put> <dir path>/<file name in system>
should provide standard functionality though.
