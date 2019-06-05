#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "circuit.h"
#include "mc.h"

using namespace std;

int main()
{
	srand(time(0));
	circuit c1,c2;
	mc run1;
	bool * in;
	int ene;
	double T_max = 1000, T_min = 1;
	int iter_max = 10000;

	in = new bool[10];
	c1.init(10,100,1);
	c1.rand_connect(0.01);
	c1.count_group();
	run1.T_max = T_max;
	run1.T_min = T_min;
	// run for c1 first
	ene = 0;
	for(int t1=0; t1<1024; t1++)
	{
		for(int t2=t1,two=512,t3=9; t3>=0; t3--)
		{
			in[t3] = t2/two;
			t2 = t2%two;
			two /= 2;
		}
		c1.run(in);
		ene += fabs(c1.output[0] - ((int)(10+10*sin(t1)))%2);
	}
	run1.ene_old = ene;

	for(int t0 = 0; t0 < iter_max; t0++)
	{
		c2 = c1;
		c2.mutate(0.01);
		c2.count_group();
		ene = 0;
		for(int t1=0; t1<1024; t1++)
		{
			for(int t2=t1,two=512,t3=9; t3>=0; t3--)
			{
				in[t3] = t2/two;
				t2 = t2%two;
				two /= 2;
			}
			c2.run(in);
			ene += fabs(c2.output[0] - ((int)(10+10*sin(t1)))%2);
		}
		run1.change_T(t0,iter_max);
		if(run1.check_if_accept(ene))
		{
			cout<<"Accept"<<endl;
			c1 = c2;
		}
		cout<<run1.ene_old<<endl;
	}
}
