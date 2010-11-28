#include "prelude.h"
#include "profiling.h"
#include "tablero.h"
#include "ia.h"
player_param def_player = (player_param){1,1,1,1,1};
bool alphabeta = false, adaptive = false;
int height = 0;

move jugada_optima(int quien, float alpha, float beta, int h) {
	bool soyyo = quien == YO, dir, empty = true;
	move mo(0, 0, soyyo ? -INFINITO : INFINITO, HORIZONTAL);
	START_PROFILING()
	if(h) {
		//Recorro todas las casillas viendo donde puedo insertar
		forn(i, n) forn(j, n) {
			if(tablero[i][j] != VACIO) continue;
			lvecino direcciones = vecinos(i, j);
			forall(it, direcciones) {
				if(profiling) PROFILE_INCREASE_CHILDREN()
				dir = *it;
				colocar(quien, i, j, dir);
				if(soyyo) {
					move next = jugada_optima(EL, mo.score, beta, h-1);
					if(next.score > mo.score) mo = move(i, j, next.score, dir);
					if(alphabeta && mo.score >= beta) {
						sacar(i, j, dir);
						if(profiling) PROFILE()
						return move(i, j, beta, dir);
					}
				} else {
					move next = jugada_optima(YO, alpha, mo.score, h-1);
					if(next.score < mo.score) mo = move(i, j, next.score, dir);
					if(alphabeta && mo.score <= alpha) {
						sacar(i, j, dir);
						if(profiling) PROFILE()
						return move(i, j, alpha, dir);
					}
				}
				sacar(i, j, dir);
			}
			empty = empty && direcciones.empty();
		}
	}
	if(profiling) PROFILE()
	// Si es terminal, calculo el puntaje del tablero
	// No hay movimientos válidos en una casilla terminal
	return !empty ? mo : move(-1, -1, (funciones[funcion])(tablero, def_player), VERTICAL);
}

move jugar(int quien, float alpha, float beta, int h) {
	bool soyyo = quien == YO, dir;
	funcion_evaluacion heuristica = funciones[funcion];
	move best(0, 0, soyyo ? -INFINITO : INFINITO, HORIZONTAL);
	if(!h) return move(-1, -1, heuristica(tablero, def_player), VERTICAL);
	lmove moves;
	forn(i, n) forn(j, n) {
		if(tablero[i][j] != VACIO) continue;
		lvecino direcciones = vecinos(i, j);
		forall(it, direcciones) {
			dir = *it;
			colocar(quien, i, j, dir);
			moves.push_back(move(i, j, heuristica(tablero, def_player), dir));
			sacar(i, j, dir);
		}
	}
	if(moves.empty()) return move(-1, -1, heuristica(tablero, def_player), VERTICAL);
	moves.sort();
	if(soyyo) moves.reverse();
	if(moves.size() > (unsigned) nx) moves.resize(nx);
	if(soyyo) {
		forall(it, moves) {
			move mo = *it;
			colocar(quien, mo.x, mo.y, mo.dir);
			move next = jugar(EL, best.score, beta, h-1);
			sacar(mo.x, mo.y, mo.dir);
			if(next.score > best.score) best = move(mo, next.score);
			if(alphabeta && best.score >= beta) return move(mo, beta);
		}
	} else {
		forall(it, moves) {
			move mo = *it;
			colocar(quien, mo.x, mo.y, mo.dir);
			move next = jugar(YO, alpha, best.score, h-1);
			sacar(mo.x, mo.y, mo.dir);
			if(next.score < best.score) best = move(mo, next.score);
			if(alphabeta && best.score <= alpha) return move(mo, alpha);
		}
	}
	return best;
}
