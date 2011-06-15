#ifndef _PRELUDE_H
#define _PRELUDE_H

#include <iostream>
#include <vector>
#include <list>
#include <signal.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>
using namespace std;

#define forn(i,n) for(int i=0;i<int(n);++i)
#define forsn(i,s,n) for(int i=s;i<int(n);++i)
#define dforn(i,n) for(int i=int(n)-1;i>=0;--i)
#define dforsn(i,s,n) for(int i=int(n)-1;i>=s;--i)
#define forall(i,c) for(typeof(c.begin()) i=c.begin();i!=c.end();++i)

#define YO 0
#define EL 1
#define VACIO (-1)
#define HORIZONTAL false
#define VERTICAL true
#define OTRO(n) (1-(n))
#define INFINITO 999999999
#define chin min
#define lin min

struct move {
	int x, y;
	bool dir;
	float score;
	move (int _x, int _y, float _score, bool _dir) : 
		x(_x), y(_y), dir(_dir), score(_score) {}
	move(){}
	move (const move& m, float new_score) : 
		x(m.x), y(m.y), dir(m.dir), score(new_score) {}
	bool operator<(const move& otro) const {
		return score < otro.score;
	}
};

struct player_param {
	float valor_puntaje;
	float valor_exp_mio;
	float valor_exp_el;
	float valor_distancia;
	float valor_divisiones;
};

typedef bool vecino;
typedef list<vecino> lvecino;
typedef vector<int> vint;
typedef vector<double> vdouble;
typedef vector<vint> vvint;
typedef float (*funcion_evaluacion)(const vvint&, const player_param&);
typedef list<move> lmove;

extern int n, height, funcion, nx;
extern bool profiling, alphabeta, adaptive, first_run;
extern player_param def_player;

#endif
