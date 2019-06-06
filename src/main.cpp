#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "circuit.h"
#include "mc.h"

using namespace std;

int target(int in)
{
	if (300<in && in < 700)
		return 0;
	else
		return 1;
}

int main()
{
	srand(time(0));
//	srand(0);
	circuit c1,c2;
	mc run1;
	int ene;
	double T_max = 2, T_min = 1;
	int iter_max = 10000;

	c1.init(10,10,1);
	c1.rand_connect();
	c1.count_group();
	run1.T_max = T_max;
	run1.T_min = T_min;
//	c1.print();
//	cout<<"-------------------------------------------------------"<<endl;
//	c1.mutate(0.5);
//	c1.print();
//	return 0;
	// run for c1 first
	ene = 0;
	for(int t1=0; t1<1024; t1++)
	{
		c1.run(t1);
		ene += fabs(c1.output[0] - target(t1));
		c1.reset_gate();
	}
	run1.ene_old = ene;

	for(int t0 = 0; t0 < iter_max; t0++)
	{
		c2 = c1;
		c2.mutate(0.1);
		c2.count_group();
		ene = 0;
		for(int t1=0; t1<1024; t1++)
		{
			c2.run(t1);
			ene += fabs(c2.output[0] - target(t1));
			c2.reset_gate();
		}
		run1.change_T(t0,iter_max);
		if(run1.check_if_accept(ene))
			c1 = c2;
		cout<<run1.ene_old<<'\t'<<t0<<endl;
	}
	//test
	cout<<"Test"<<endl;
	for(int t1=0; t1<1024; t1++)
	{
		c1.run(t1);
		ene += fabs(c1.output[0] - target(t1));
		cout<<t1<<'\t'<<target(t1)<<'\t'<<c1.output[0]<<endl;
		c1.reset_gate();
	}
	return 0;
}
