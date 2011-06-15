#include "prelude.h"
#include "ia.h"
#include "profiling.h"
#include "tablero.h"
#include "juego.h"

bool first_run = true, profiling;
int n, nx, funcion = 0, k;
float x;

void fill_player(player_param& p, char* argv[], int& i, int argc) {
	if (i < argc) p.valor_puntaje = atof(argv[++i]);
	if (i < argc) p.valor_exp_mio = atof(argv[++i]);
	if (i < argc) p.valor_exp_el = atof(argv[++i]);
	if (i < argc) p.valor_distancia = atof(argv[++i]);
	if (i < argc) p.valor_divisiones = atof(argv[++i]);
}

void do_adaptive(char** argv, int& i) {
	x = atof(argv[++i]);
	k = atoi(argv[++i]);
	height = 2*(k-2)/x; // quiero cumplir h*x <= 2(k-2)
	adaptive = true;
}

void adaptive_info() {
	cerr << "n = " << n << ". ";
	cerr << "Using adaptive depth = " << height;
	cerr << ", seeing n^" << x << " = " << nx << " children per node";
	cerr << " (out of a possible " << 2*n*(n-1) << "). ";
	cerr << "Meets O(n^{2k}) = O(n^{2*" << k << "})";
	cerr << " = O(n^" << 2*k << ")." << endl;
}

void do_help(char* name) {
	cerr << "Usage: " << name << " [options]" << endl;
	cerr << "Options are:" << endl;
	cerr << "\t--help\t\t\tShow this help message and quit." << endl;	
	cerr << "\t--profiling\t\tTurns on profiling, outputs to standard error." << endl;
	cerr << "\t--alphabeta\t\tTurns on alpha-beta pruning." << endl;
	cerr << "\t--height h\t\tSets the search depth to an integer h." << endl;
	cerr << "\t--funcion i\t\tUses the i'th heuristic in the internal \"funciones\" array." << endl;
	cerr << "\t--adaptive x k\t\tUses adaptive depth searching, looking at n^x children for each node, and meeting O(n^{2k})." << endl;
	cerr << "\t--params a b c d e\tChanges the heuristic values of each of the 5 floating point parameters:" << endl;
	{
		cerr << "\t\t\t\t\ta is the score for the board's numerical value." << endl;
		cerr << "\t\t\t\t\tb is the score for my player's expansion." << endl;
		cerr << "\t\t\t\t\tc is the score for my opponent's expansion. This value is multiplied by -1." << endl;
		cerr << "\t\t\t\t\td is the score for my distance to the walls." << endl;
		cerr << "\t\t\t\t\te is the score for my division heuristic." << endl;
	}
	exit(0);
}

int main(int argc, char* argv[]) {
	signal(SIGTERM, terminar);
	srand(time(NULL));
	forn(i, argc) {
		if(strcmp(argv[i], "--help") == 0) do_help(argv[0]);
		else if(strcmp(argv[i], "--profiling") == 0 || strcmp(argv[i], "-p") == 0) profiling = true;
		else if(strcmp(argv[i], "--alphabeta") == 0 || strcmp(argv[i], "-ab") == 0) alphabeta = true;
		else if(strcmp(argv[i], "--height") == 0 && argc > i+1) height = atoi(argv[i+1]);
		else if(strcmp(argv[i], "--funcion") == 0 && argc > i+1) funcion = atoi(argv[i+1]);
		else if(strcmp(argv[i], "--adaptive") == 0 && argc > i+2) do_adaptive(argv, i);
		else if(strcmp(argv[i], "--params") == 0) fill_player(def_player, argv, i, argc);
	}
       /* Si c==1, arrancamos leyendo una jugada del otro */
	int c;
	cin >> n >> c;
	if(adaptive) {
		nx = (int) pow((float) n, x);	
		adaptive_info();
	}
	tablero.resize(n, vint(n, VACIO));
	if(profiling) ENABLE_PROFILING(n)
	else DISABLE_PROFILING()
	if(height <= 0) height = n*n/2+1;
	if (n>1 && c) lee_movida();
	while (hay_movida()) {	
		juega_movida();
		// Si no hay movida para hacer se cuelga leyendo, pero el judge nos mata el proceso con SIGTERM
		lee_movida();
	}
	return 0;
}
