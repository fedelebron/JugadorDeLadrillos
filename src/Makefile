CC = g++
CXXFLAGS = -O3 -g -Wall -Werror
PLAYERS = jugador
PLAYGROUND = ../test/playground/jugadores_ours/alphabeta
OBJS=profiling.o tablero.o juego.o ia.o jugador.o evaluacion.o
.PHONY: all

all: jugador

profiling.o: profiling.h
evaluacion.o: isle_utils.cpp

jugador: $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o jugador

to_playground: $(PLAYERS)
	cp $(PLAYERS) $(PLAYGROUND)

clean:
	rm -f jugador
	rm -f *.o
