#include <cassert>

vvint islas;
vint tamanios;

int isla(int i, int j) {
	return islas[i][j];
}

int cantidad(int i, int j) {
	return tamanios[isla(i, j)];
}