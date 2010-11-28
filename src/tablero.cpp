#include "tablero.h"

vvint tablero;

//Modifica el tablero agregando la ficha
//agrega un nodo correspondiente a ptr
//devuelve el nodo agregado
void colocar(int quien, int x, int y, bool dir) {
	tablero[x][y] = quien;
	tablero[x+dir][y+1-dir] = quien;
}

// Sacar una ficha del tablero tempral "tablero"
void sacar(int x, int y, bool dir) {
	colocar(VACIO, x, y, dir);
}

// Dar las posibles formas de poner una ficha en una casilla.
lvecino vecinos(int i, int j) {
	lvecino l;
	if(j != n-1 && tablero[i][j+1] == VACIO) l.push_back(HORIZONTAL);
	if(i != n-1 && tablero[i+1][j] == VACIO) l.push_back(VERTICAL);
	return l;
}