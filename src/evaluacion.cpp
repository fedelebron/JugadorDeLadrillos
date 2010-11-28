#include <iostream>
#include <vector>
#include <list>
#include <signal.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <math.h>

using namespace std;

#include "prelude.h"

// Dado un tablero temporal "tablero", correspondiente 
// a un juego terminado, calcula el puntaje para 
// mi jugador. O(N^2).
#include "isle_utils.cpp"

int dfs(const vvint& tablero, int i, int j, int quien, int isla) {
	if(quien == VACIO || islas[i][j] != -1 || tablero[i][j] != quien) return 0;
	int n = tablero.size(), puntos = 1;
	islas[i][j] = isla;
	if(i) puntos += dfs(tablero, i-1, j, quien, isla);
	if(j) puntos += dfs(tablero, i, j-1, quien, isla);
	if(i < n-1) puntos += dfs(tablero, i+1, j, quien, isla);
	if(j < n-1) puntos += dfs(tablero, i, j+1, quien, isla);
	return puntos;
}

float puntaje_lineal(const vvint& tablero, const player_param& params) {
	int n = tablero.size(), puntaje = 0, isla = -1, quien, island_size;
	islas.clear();
	islas.resize(n, vint(n, -1));
	tamanios.clear();
	forn(i, n) forn(j, n) {
		quien = tablero[i][j];
		if(quien == VACIO || islas[i][j] != -1) continue;
		island_size = dfs(tablero, i, j, quien, ++isla);
		puntaje += island_size*island_size*(1 - 2*quien);
		tamanios.push_back(island_size);
	}
	return (float) puntaje;
}

int divisiones(const vvint& tablero){
	int n = tablero.size(), l, isle;
	int com = floor((n - sqrt(n))/2);
	int fin = ceil(com + sqrt(n));
	int lim = floor(n/2);
	int a, b, c, d, ab, bb, cb, db;
	int i0k, ik0, i0l, il0, ink, inl, ikn, iln;
	a = b = c = d = ab = bb = cb = db = 0;
	forn(k, lim) {
		l = lim+k;
		i0k = isla(0, k);
		i0l = isla(0, l);
		ik0 = isla(k, 0);
		il0 = isla(l, 0);
		ink = isla(n-1, k);
		inl = isla(n-1, l);
		ikn = isla(k, n-1);
		iln = isla(l, n-1);
		forsn(i, com, fin) {
			forsn(j, com, fin) {
				if (tablero[i][j] != YO) continue;
				isle = isla(i, j);
				if (isle == i0k) a = n;
				if (isle == i0l) ab = n;
				if (isle == ik0) b = n;
				if (isle == il0) bb = n;
				if (isle == ink) c = n;
				if (isle == inl) cb = n;
				if (isle == ikn) d = n;
				if (isle == iln) db = n;
			}
		}
	}
	return a+b+c+d+ab+bb+cb+db;
}

int expansion(const vvint& tablero, int jugador) {
	// Esta función calcula la cantidad de casilleros vacíos adyacentes a una isla 
	int n = tablero.size(), res = 0, isle;
	forn(i, n) forn(j, n) {
		if (tablero[i][j] != VACIO) continue;
		isle = 0;
		if (i && tablero[i-1][j] == jugador) isle = max(isle, cantidad(i-1, j));
		if (i < n-1 && tablero[i+1][j] == jugador) isle = max(isle, cantidad(i+1, j));
		if (j && tablero[i][j-1] == jugador) isle = max(isle, cantidad(i, j-1));
		if (j < n-1 && tablero[i][j+1] == jugador) isle = max(isle, cantidad(i, j+1));
		res += isle;
	}
	return res;
}

int dist(const vvint& tablero, int jugador) {
	int sum = 0, n = tablero.size();
	forn(i, n) forn(j, n) if(tablero[i][j] == jugador) sum += min(i, chin(j, lin(n-i, n-j)));
	return sum;
}

float heur(const vvint& tablero, const player_param& params) {
	float x = puntaje_lineal(tablero, params);
	return (x*params.valor_puntaje
	 		+ dist(tablero, YO)*params.valor_distancia
			+ expansion(tablero, YO)*params.valor_exp_mio
			- expansion(tablero, EL)*params.valor_exp_el
		 	+ divisiones(tablero)*params.valor_divisiones);
}	

funcion_evaluacion funciones[] = {&puntaje_lineal, &heur};