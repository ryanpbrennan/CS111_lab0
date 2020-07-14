CC = gcc
CFLAGs = -Wall -Wextra
TARGET = lab1a
TARBALL = $(TARGET)-605347597.tar.gz 

lab1a: lab1a.c
	$(CC) $(TARGET).c -g $(CFLAGS) -o $(TARGET) 

clean:
	rm -f $(TARGET) $(TARGET)-605347597.tar.gz *.o

dist:
	tar -cvzf $(TARBALL) $(TARGET).c Makefile README
