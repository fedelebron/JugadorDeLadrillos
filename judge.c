#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <sys/socket.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#define enrango(a,x,b) ((a) <= (x) && (x) < (b))
#define forn(i,n) for(i=0; i < (int)(n); ++i)

#define __mac_xstr(s) #s
#define __mac_str(s) __mac_xstr(s)
//#define DBG(X ...) fprintf(stderr, "JUDGE:"__mac_str(__FILE__)":"__mac_str(__LINE__)": "X)
#define DBG(X ...)

#define RESET_COLOR "\e[m"
#define GREEN  "\033[22;32m"
#define RED  "\033[22;31m"
#define TIMELIMIT 2400


/*** Player ***/

const char* playername[2] = {"rojo", "verde"};

typedef struct str_player {
	int pin[2], pout[2], id;
	struct timeval reloj;
	pid_t pd;
} player;

int player_launch(player* this, int k, const char* proc, char* argv[]) {
	DBG("player_launch(%d, \"%s\")\n", k, proc);
	this->id = k;
	if (pipe(this->pin) < 0) { perror("Creando el pipe"); return -1; }
	if (pipe(this->pout) < 0) { perror("Creando el pipe"); close(this->pin[0]); return -1; }
	pid_t pd = fork();
	if (pd < 0) { perror("fork()"); return -1; }
	if (pd == 0) {
		/* Hijo */
		close(this->pin[1]);
		dup2(this->pin[0], 0);
		close(this->pin[0]);
		close(this->pout[0]);
		dup2(this->pout[1], 1);
		close(this->pout[1]);
		execvp(proc, argv);
		perror("Error al ejecutar el jugador");
		char** p;
		fprintf(stderr, "... en jugador: '%s' [ ", proc); for(p=argv; *p; p++) fprintf(stderr, "%s ", *p); fprintf(stderr, "]\n");
		exit(1);
	} else {
		/* Padre */
		close(this->pin[0]);
		close(this->pout[1]);
		this->pd = pd;
		return pd;
	}
}

int player_start(player* this, int n) {
	char buf[128];
	int st=0, ret, prn = snprintf(buf, 128, "%d %d\n", n, this->id);
	DBG("player_start(%d, \"%d, %d\")\n", this->id, n, this->id);
	if (prn < 0) { perror("snprintf"); return -1; }
	while (st < prn) if ((ret = write(this->pin[1], buf, prn)) < 0) { perror("write init"); return -1; } else { st += ret; }
	return 0;
}

int player_read(player* this, int*i, int*j, int*d) {
	char buf[128];
	int st=0, ret, lf=0;
	fd_set rdfs;
	do {
		FD_ZERO(&rdfs);
		//FD_SET(0, &rdfs);
		FD_SET(this->pout[0], &rdfs);
		ret = select(this->pout[0]+1, &rdfs, NULL, NULL, &(this->reloj));
		if (ret < 0) { perror("select for read"); return -1; }
		if (FD_ISSET(this->pout[0], &rdfs)) {
			ret = read(this->pout[0], buf+st, 127-st);
			if (ret < 0) { perror("read"); return -1; }
			if (ret == 0) { fprintf(stderr, "Pipe closed?\n"); return -1; }
			while(!lf && st < ret) lf = lf || buf[st++] == '\n';
		} else {
			fprintf(stderr, "%ld, %ld\n", (long int) this->reloj.tv_sec, (long int) this->reloj.tv_usec);
			fprintf(stderr, "Time limit exceded\n");
			return -1;
		}
	} while(!lf);
	buf[st] = 0;
	if (EOF == sscanf(buf, "%d %d %d", i, j, d)) {
		perror("sscanf");
		fprintf(stderr, "...in \"%s\"", buf);
		return -1;
	}
	(*i)--; (*j)--;
	DBG("buf: %s", buf);
	return 0;
}

int player_write(player* this, int i, int j, int d) {
	char buf[128];
	int st=0, ret, prn = snprintf(buf, 128, "%d %d %d\n", i+1, j+1, d);
	DBG("player_write(%d %d %d)\n", i+1, j+1, d);
	if (prn < 0) { perror("snprintf"); return -1; }
	while (st < prn) if ((ret = write(this->pin[1], buf, prn)) < 0) { perror("write move"); return -1; } else { st += ret; }
	return 0;
}

int player_term(player* this) {
	int ret = kill(this->pd, SIGTERM);
	if (ret!=0) {
		perror("killing_player");
	}
	return ret;
}

/*** Game ***/

typedef struct str_game {
	player pl[2];
	int sco[2];
	int n, cp, lmi, lmj, lmd;
	int** tbl;
	char *proc2, **argv2;
} game;

void game_init(game* this, int n, int tl, char* proc1, char* argv1[], char* proc2, char* argv2[]) {
	int i;

	player_launch(this->pl+0, 0, proc1, argv1);
	player_start(this->pl+0, n);
	this->pl[0].reloj = (struct timeval){ tl, 0 };

	this->proc2 = proc2;
	this->argv2 = argv2;
	this->pl[1].reloj = (struct timeval){ tl, 0 };

	this->lmi = this->lmj = this->lmd = -1;

	this->n = n;
	this->tbl = (int**)malloc(n*sizeof(int*));
	forn(i,n) memset(this->tbl[i] = (int*)malloc(n*sizeof(int)), 0xff, n*sizeof(int));
	this->cp = 0;
}

void game_end(game* this) {
	int i, st;
	forn(i,this->n) free(this->tbl[i]);
	free(this->tbl);
	player_term(this->pl+0);
	player_term(this->pl+1);
	forn(i,2) {
		while (waitpid(this->pl[i].pd, &st, 0) >= 0) {
			if (WIFEXITED(st)) break;
		}
	}
}

int ddi[4] = {-1, 1, 0, 0};
int ddj[4] = {0, 0, -1, 1};

void game_score(game* this) {
	int d, ci, cj, n=this->n;
	int*q = (int*)malloc(sizeof(int)*n*n*2);
	int*mp = (int*)malloc(sizeof(int)*n*n);
	memset(mp, 0, sizeof(int)*n*n);
	this->sco[0] = this->sco[1] = 0;
	forn(ci, n) forn(cj, n) if (!mp[ci*n+cj] && this->tbl[ci][cj] != -1) {
		int cs = 0;
		int qp = 0, qe = 0, cu = this->tbl[ci][cj]%2;
		#define pone(i,j) { mp[(i)*n+(j)]=1; q[qe++] = (i); q[qe++] = (j); }
		pone(ci,cj)
		while (qp < qe) {
			int i = q[qp++], j = q[qp++];
			cs++;
			forn(d,4) {
				int ii = i+ddi[d], jj = j+ddj[d];
				if (enrango(0,ii,n) && enrango(0,jj,n) && this->tbl[ii][jj] != -1 && this->tbl[ii][jj]%2 == cu && !mp[ii*n+jj]) { pone(ii,jj); }
			}
		}
		this->sco[cu] += cs*cs;
		#undef pone
	}
	free(q);
	free(mp);
}

int game_movida(game* this, int i, int j, int d, int k) {
	if (!(0 <= d && d < 2 && 0 <= i && i < this->n-d && 0 <= j && j < this->n-1+d)) {
		fprintf(stderr, "JUDGE: \"%d %d %d\" no es una jugada válida.\n", i+1,j+1,d); 
		return -1;
	}
	if (this->tbl[i][j] != -1 || this->tbl[i+d][j+1-d] != -1) {
		fprintf(stderr, "JUDGE: \"%d %d %d\" se pisa con una pieza ya colocada.\n", i+1,j+1,d); 
		return -1;
	}
	this->tbl[i][j] = k;
	this->tbl[i+d][j+1-d] = k;
	return 0;
}

int game_hay_movida(game* this) {
	int i, j, n = this->n;
	forn(i,n) forn(j,n-1) if (this->tbl[i][j] == -1 && this->tbl[i][j+1] == -1) return 0;
	forn(i,n-1) forn(j,n) if (this->tbl[i][j] == -1 && this->tbl[i+1][j] == -1) return 0;
	return -1;
}

void game_show(game* this) {
	int i, j, n = this->n;
	forn(i,n) { 
		forn(j,n) {
			if (this->tbl[i][j]+1 == 1) fprintf(stderr, RED);
			if (this->tbl[i][j]+1 == 2) fprintf(stderr, GREEN);
			fprintf(stderr, "%c", ".#X"[this->tbl[i][j]+1]); 
			fprintf(stderr, RESET_COLOR);
		}
		fprintf(stderr, "\n"); 
	}
	fprintf(stderr, "JUDGE: P#: %4d  | PX: %4d\n\n", this->sco[0], this->sco[1]);
}


int game_step(game* this) {
	int ret;
	player* p = this->pl+this->cp;

	/* Launch on-demand */
	if (this->proc2 && this->cp==1) {
		player_launch(this->pl+1, 1, this->proc2, this->argv2);
		player_start(this->pl+1, this->n);
		this->proc2 = 0;
	}
	
	if (game_hay_movida(this) < 0) return -0x7337;
	fprintf(stderr, "JUDGE: Player %d - TL: %ld:%.2ld.%.3ld\n", this->cp, p->reloj.tv_sec/60, p->reloj.tv_sec%60, (long int) p->reloj.tv_usec/1000);
	if (this->lmd != -1) {
		player_write(p, this->lmi, this->lmj, this->lmd);
	}
	ret = player_read(p, &(this->lmi), &(this->lmj), &(this->lmd));
	if (ret < 0) { return ret; }  
	ret = game_movida(this, this->lmi,this->lmj,this->lmd, this->cp);
	if (ret < 0) { return ret; }  
	this->cp = 1-this->cp;
	game_score(this);
	game_show(this);
	return 0;
}

int signo(int x) { return (x>0)-(x<0); }

void game_play(game* this) {
	int ret, win;
	while (!(ret = game_step(this)));
	if (ret != -0x7337) {
		fprintf(stderr, "JUDGE: Player %d abandona\n", this->cp);
		win = this->cp*2-1;
		printf("%d %d %.3lf %.3lf\n", this->sco[0], this->sco[1], (double)(this->n*this->n*win), (double)(this->n*this->n*(-win)));
	} else {
		win = signo(this->sco[0] - this->sco[1]);
	
		if (!win) {
			fprintf(stderr, "JUDGE: Empate!\n");
		} else { 
			fprintf(stderr, "JUDGE: Ganador: Player %s (%d)\n", playername[(1-win)/2], (1-win)/2);
		}
		double diff = sqrt((double)abs(this->sco[0]-this->sco[1]));
		printf("%d %d %.3lf %.3lf\n", this->sco[0], this->sco[1], win*diff, -win*diff);
	}
}

/*** Main functions ***/

void usage(char* argv[]) {
	fprintf(stderr, "Usage: %s <n> <player1> [<player1_opt> ... <player1_opt> --] <player2> [<player2_opt> ... <player2_opt>]\n", argv[0]);
	exit(0);
}

int main(int argc, char* argv[]) {
	int i, n = 5;
	int timelimit = TIMELIMIT; // en segundos
	char **l1, **l2, *argv1[2] = {0,0};
	int dash = 0;
	game g;

	if (argc < 4) usage(argv);
	n = atoi(argv[1]);
	for(i=2; i < argc; i++) if (!strcmp("--", argv[i])) { dash = i; break; }
	if (dash) {
		argv[dash] = 0;
		l1 = argv+2;
		l2 = argv+dash+1;
	} else {
		l1 = argv1;
		argv1[0] = argv[2];
		l2 = argv+3;
	}

	game_init(&g, n, timelimit, l1[0], l1, l2[0], l2);
	game_play(&g);
	game_end(&g);
	return 0;
}
