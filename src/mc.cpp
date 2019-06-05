#include <cstdlib>
#include <cmath>
#include "mc.h"

using namespace std;

bool mc :: check_if_accept(double ene_new)
{
	double exp_main = exp(-(ene_new-ene_old)/T);
	if (rand()/(double)RAND_MAX < exp_main)
	{
		ene_old = ene_new;
		return true;
	}
	else
		return false;
}

void mc :: change_T(int iter, int iter_max)
{
	T = T_max - (T_max - T_min)*(double)iter/iter_max;
}
