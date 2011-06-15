#ifndef TABLERO_H
#define TABLERO_H
#include "prelude.h"
void colocar(int quien, int x, int y, bool dir);
void sacar(int x, int y, bool dir);
lvecino vecinos(int i, int j);
extern vvint tablero;
#endif