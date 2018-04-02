.phony all:
all: filesys

filesys: master.c
	gcc -Wall -D PART1 master.c -o diskinfo
	gcc -Wall -D PART2 master.c -o disklist
	gcc -Wall -D PART3 master.c -o diskget
	gcc -Wall -D PART4 master.c -o diskput
	
part: master.c
	gcc -Wall -D PART4 master.c -o diskget


.PHONY clean:
clean:
	-rm diskinfo disklist diskget diskput
