#include <cstdio>
#include <iostream>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <set>
#include <sys/time.h>
using namespace std;
#define forn(i,n) for(int i=0;i<int(n);++i)
#define forsn(i,s,n) for(int i=s;i<int(n);++i)
#define dforn(i,n) for(int i=int(n)-1;i>=0;--i)
#define dforsn(i,s,n) for(int i=int(n)-1;i>=s;--i)
#define forall(i,c) for(typeof(c.begin()) i=c.begin();i!=c.end();++i)

int rnd(int n) { return rand()%n; }
int rnd(int a, int b) { return a+rnd(b-a+1); }
int rndSmall(int a, int b) { return rnd(a,rnd(a,b)); }
int rndBig(int a, int b) { return rnd(rnd(a,b),b); }


char toCh(int x) { return ".#O"[x+1]; }

void print(const vector< vector<int> >& st) {
	forn(i,st.size()) {
		forn(j,st[i].size()) cerr << toCh(st[i][j]);
		cerr << endl;
	}
}

bool valid(const vector< vector<int> >& st, int i, int j, int d) {
	int n=st.size();
	return 0 <= d && d < 2 && 0 <= i && i < n-d && 0 <= j && j < n-1+d;
}

bool used(const vector< vector<int> >& st, int i, int j, int d, int c) {
	return st[i][j] == c && st[i+d][j+1-d] == c;
}

bool pos(const vector< vector<int> >& st, int i, int j, int d) {
	return valid(st,i,j,d) && used(st,i,j,d,-1);
}

void add(vector< vector<int> >& st, int i, int j, int d, int c) {
	assert(valid(st,i,j,d));
	assert(pos(st,i,j,d));
	assert(0 <= c && c < 2);
	st[i][j]=st[i+d][j+1-d]=c;
}

void remove(vector< vector<int> >& st, int i, int j, int d, int c) {
	assert(0 <= c && c < 2);
	assert(used(st,i,j,d,c));
	st[i][j]=st[i+d][j+1-d]=-1;
}

void esperar(vector< vector<int> >& st, int c) {
	int i,j,d;
	cin >> i >> j >> d;
	add(st,i-1,j-1,d,1-c);
}

typedef void(*calc)(const vector< vector<int> >&,int&,int&,int&,int);

void jugar(vector< vector<int> >& st, int c, calc cF) {
	int i,j,d;
	cF(st, i, j, d, c);
	add(st,i,j,d,c);
	cout << i+1 << " " << j+1 << " " << d << endl;
}

void calcFijo(const vector< vector<int> >& st, int& i, int& j, int& d, int) {
	int n=st.size();
	forn(pi,n)forn(pj,n)forn(pd,2) if (pos(st,pi,pj,pd)) {
		i=pi,j=pj,d=pd;
		return;
	}
}
void calcRand(const vector< vector<int> >& st, int& i, int& j, int& d, int) {
	int n=st.size();
	int t=0;
	forn(pi,n)forn(pj,n)forn(pd,2) if (pos(st,pi,pj,pd)) {
		++t;
		if (!rnd(t)) i=pi,j=pj,d=pd;
	}
}

#define inf 1000000000

int dfs(int i, int j, vector< vector<int> >& st, int c) {
	if (i < 0 || i >= int(st.size()) || j < 0 || j >= int(st.size())) return 0;
	if (st[i][j] != c) return 0;
	st[i][j]+=2;
	return 1+dfs(i+1,j,st,c)+dfs(i,j+1,st,c)+dfs(i-1,j,st,c)+dfs(i,j-1,st,c);
}

typedef int(*points)(vector< vector<int> >&, int);

int dummy_points(vector< vector<int> >& st, int c) {
	int n=st.size(),r=0;
	forn(i,n)forn(j,n) if (st[i][j]==0 || st[i][j]==1) {
		 int a = dfs(i,j,st,st[i][j]);
		 r+=(st[i][j]==c+2?1:-1)*a*a;
	}
	forn(i,n)forn(j,n) if (st[i][j]>=0) st[i][j]-=2;
	return r;
}

int bdfs(int i, int j, vector< vector<int> >& st, int c, set< pair<int,int> >& per) {
	if (i < 0 || i >= int(st.size()) || j < 0 || j >= int(st.size())) return 0;
	if (st[i][j] == -1) { per.insert(make_pair(i,j)); return 0; }
	if (st[i][j] != c) return 0;
	st[i][j]+=2;
	return 1+bdfs(i+1,j,st,c,per)+bdfs(i,j+1,st,c,per)+bdfs(i-1,j,st,c,per)+bdfs(i,j-1,st,c,per);
}

int better_points(vector< vector<int> >& st, int c) {
	int n=st.size(),r=0;
	forn(i,n)forn(j,n) if (st[i][j]==0 || st[i][j]==1) {
		 set< pair<int,int> > per;
		 int a = bdfs(i,j,st,st[i][j],per);
		 r+=(st[i][j]==c+2?1:-1)*a*a*per.size();
	 }
	forn(i,n)forn(j,n) if (st[i][j]>=0) st[i][j]-=2;
	return r;
}

void calcHeur(const vector< vector<int> >& st, int& i, int& j, int& d, int c, points pF) {
	int n=st.size();
	int cp = -inf;
	vector< vector<int> > stp(st);
	int t=0;
	forn(pi,n)forn(pj,n)forn(pd,2) if (pos(st,pi,pj,pd)) {
		add(stp,pi,pj,pd,c);
		int p = pF(stp,c);
		//print(stp);
		//cerr << "po = " << p << " " << endl;
		if (p > cp) i=pi,j=pj,d=pd,t=0,cp=p;
		if (p == cp) {
			if (!rnd(++t)) i=pi,j=pj,d=pd;
		}
		remove(stp,pi,pj,pd,c);
	}
}

void calcHeurDummy(const vector< vector<int> >& st, int& i, int& j, int& d, int c) {
	calcHeur(st,i,j,d,c,dummy_points);
}

void calcHeurBetter(const vector< vector<int> >& st, int& i, int& j, int& d, int c) {
	calcHeur(st,i,j,d,c,better_points);
}


#define JUGADOR_FIJO 0
#define JUGADOR_RANDOM 1
#define JUGADOR_HEURISTICO 2
#define JUGADOR_HEURISTICO_MEJOR 3

calc calcs[] = {calcFijo, calcRand, calcHeurDummy, calcHeurBetter};
const char* names[] = {"FIJO", "RANDOM","HEURISTICO", "HEURISTICO MEJOR"};

int main(int argc, char** argv) {
	if (argc > 1) {
		if (argc == 2 && string(argv[1]) ==  "-v") {
			cerr << names[JUGADOR] << endl;
			return 0;
		} else {
			cerr << "Usage: " << argv[0] << " [-v]" << endl;
			//return 1;
		}
	}
	struct timeval r_time;
	gettimeofday(&r_time, NULL);
	srand((int)r_time.tv_usec);
	int n,c;
	cin >> n >> c;
	vector< vector<int> > st(n, vector<int>(n, -1));
	if (c==1) esperar(st,c);
	while(true) {
		jugar(st, c, calcs[JUGADOR]);
		esperar(st,c);
	}
	return 0;
}
