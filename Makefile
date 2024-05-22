.PHONY = all, clean
cc = gcc
CFLAGS = -std=c11 -pedantic -Wall -Werror -D_XOPEN_SOURCE=700 -g -fanalyzer
all: clash
clash: clash.o
	$(cc) clash.o -o clash $(CFLAGS)
clash.o: clash.c plist.o
	$(cc) clash.c -o clash.o -c $(CFLAGS)
plist.o: plist.c plist.h
	$(cc) plist.c -o plist.o -c $(CFLAGS)
clean:
				rm -f clash plist.o clash.o
