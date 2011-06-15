export LC_ALL=C
CC=gcc
CXX=g++
CFLAGS=-Wall -O2  -g -ggdb
CXXFLAGS=-Wall -O2 -g -ggdb
LDFLAGS=

OBJS=judge.o
BINS=judge

P_OURS = jugadores_ours
P_THEIRS = jugadores_cat

all: $(BINS)
	make -C $(P_OURS) 
	make -C $(P_THEIRS) 

judge: judge.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

judge.o: judge.c

clean:
	rm -f $(OBJS) $(BINS)
	make -C $(P_THEIRS) clean
	make -C $(P_OURS) clean

