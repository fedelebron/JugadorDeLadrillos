#include <iostream>
#include <vector>
#include <cstdlib>
#include <signal.h>

using namespace std;

typedef vector<int> vint;
typedef vector<vint> vvint;

#define forn(i,n) for(int i=0; i < (int)(n); ++i)

vvint tbl;
int n;

/* Función para mostrar el tablero en stderr */
void board_show() {
	forn(i,n) { forn(j,n) cerr << ".#X"[tbl[i][j]+1]; cerr << endl; }
}

void aplica_movida(int i, int j, int d, int k) {
	tbl[i][j] = k;
	tbl[i+d][j+1-d] = k;
}

void lee_movida(int k) {
	int i,j,d;
	cin >> i >> j >> d; i--; j--;
	/* No es necesario checkear que la jugada sea válida, estamos seguros */
	aplica_movida(i,j,d,k);
}

int hay_movida() {
	forn(i,n) forn(j,n-1) if (tbl[i][j] == -1 && tbl[i][j+1] == -1) return true;
	forn(i,n-1) forn(j,n) if (tbl[i][j] == -1 && tbl[i+1][j] == -1) return true;
	return false;
}

void juega_movida(int k) {
	forn(x,n) forn(y,n-1) {
		if (tbl[x][y] == -1 && tbl[x][y+1] == -1) { aplica_movida(x,y,0,k); cout << x+1 << " " << y+1 << " " << 0 << endl; return; }
		if (tbl[y][x] == -1 && tbl[y+1][x] == -1) { aplica_movida(y,x,1,k); cout << y+1 << " " << x+1 << " " << 1 << endl; return; }
	}
}

/* Cuando se reciba un SIGTERM se ejecutará esta función. No es necesario
 * hacer nada acá, pero si alguno lo necesita puede hacer alguna tarea acá,
 * como guardar estadísticas del juego, etc. */
void terminar(int) {
	cerr << "Uy!, se acabó el partido!" << endl;
	exit(0);
}

int main(int argc, char* argv[]) {
	signal(SIGTERM, terminar);
	int c;
	cin >> n >> c;
	tbl = vvint(n, vint(n, -1));
	/* Si c==1, arrancamos leyendo una jugada del otro */
	if (n>1 && c) lee_movida(!c);
	while (hay_movida()) {
		juega_movida(c);
		// Si no hay movida para hacer se cuelga leyendo, pero el judge nos mata el proceso con SIGTERM
		lee_movida(!c); 
	}
	return 0;
}
