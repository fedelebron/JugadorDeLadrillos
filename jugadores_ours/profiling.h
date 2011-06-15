#ifndef PROFILING_H
#define PROFILING_H

#include "prelude.h"

extern vint nodos, minimos, maximos;
extern vdouble promedios;
extern int nivel;

#define START_PROFILING() 	int children = 0;\
							if(first_run) ++nivel;
							
#define PROFILE() if(first_run) {\
	++nodos[--nivel];\
	promedios[nivel] = (promedios[nivel]*(nodos[nivel]-1) + children)/nodos[nivel];\
	maximos[nivel] = max(maximos[nivel], children);\
	minimos[nivel] = min(minimos[nivel], children);\
};

#define PROFILE_INCREASE_CHILDREN() if(first_run) ++children;

#define PRINT_PROFILING() {\
	int sum = 0;\
	forn(i, n*n) {\
		if(nodos[i]) {\
			cerr << "Nivel " << i << ":" << endl;\
			cerr << "\tnodos: " << nodos[i] << endl;\
			cerr << "\tmaximo: " << maximos[i] << endl;\
			cerr << "\tminimo: " << minimos[i] << endl;\
			cerr << "\tpromedio: " << promedios[i] << endl;\
			sum += nodos[i];\
		}\
	}\
	cerr << "Total: " << sum << endl;\
}

#define ENABLE_PROFILING(n) {\
	promedios.resize(n*n, 0);\
	maximos.resize(n*n, 0);\
	minimos.resize(n*n, INFINITO);\
	nodos.resize(n*n, 0);\
}

#define DISABLE_PROFILING() first_run = false;

#endif
