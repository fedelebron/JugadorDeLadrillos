#include "prelude.h"
#include "profiling.h"
#include "juego.h"
#include "tablero.h"
#include "ia.h"

move nextmove(-1, -1, -1, VERTICAL);

bool hay_movida() {
	if(adaptive) nextmove = jugar(YO, -INFINITO, INFINITO, height);
	else nextmove = jugada_optima(YO, -INFINITO, INFINITO, height);
	if(profiling) DISABLE_PROFILING()
	return nextmove.x != -1;
}


void lee_movida() {
	int x, y, dir;
	cin >> x >> y >> dir;
	colocar(EL, x-1, y-1, dir);
}

//Asume que hay movidas
void juega_movida() {
	colocar(YO, nextmove.x, nextmove.y, nextmove.dir);
	cout << nextmove.x + 1 << " " << nextmove.y + 1 << " " << nextmove.dir << endl;
}

void terminar(int x) {
	if(profiling) {
		PRINT_PROFILING()
	}
	exit(x);
}
