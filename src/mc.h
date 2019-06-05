#ifndef __MC__
#define __MC__

class mc
{
public:
	double T, T_max, T_min;
	double ene_old;
	bool if_accept;
	
	bool check_if_accept(double ene_new);
	void change_T(int iter, int iter_max);
};

#endif
