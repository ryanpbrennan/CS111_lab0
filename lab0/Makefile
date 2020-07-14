CC = gcc
CFLAGs = -Wall -Wextra
TARGET = lab0
TARBALL = $(TARGET)-605347597.tar.gz 

lab0: lab0.c
	$(CC) $(TARGET).c -g $(CFLAGS) -o $(TARGET) 
check:
	./smokehelper.sh

clean:
	rm -f lab0 lab0-605347597.tar.gz *.o t1.txt t2.txt

dist:
	tar -cvzf $(TARBALL) $(TARGET).c Makefile smoketest.sh smokehelper.sh README *.png
