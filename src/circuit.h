#ifndef __CIRCUIT__
#define __CIRCUIT__

#include "gate.h"
#include <vector>

class circuit
{
private:
	std::vector <gate_nand> list_g;
	std::vector <std::vector <int> > group;
	std::vector <int> to_run;
	std::vector <std::vector <int> > l_group; // to find which group the pin is in
public:
	int num_i;
	int num_m;
	int num_o;
	int num_tot;
	std::vector <int> output;
	//=======function====================
	void init(int in, int middle, int out);
	
	void rand_connect(double p_new); // probability of creating a new group
	// Connect rule:
	/*
	1.	Two outputs cannot connect together
		(So can start with number of groups equal number of gates,
		each group start with one out, can they never change group.
		New group that contains purely in's can be created or deleted)
	2.	Inputs cannot connect to self outputs (can change)
	3.	May consider "always on" and "always off" group
	*/
	
	void count_group();
	void reset_gate();

	void run(bool *& input);
	// Running rule:
	/*
	1.	Inputs cannot directly change outputs
	2.	One gate at most runs once
	3.	The second input of all in_gates are set to false (can change this rule)
	*/

	void mutate(double p_new);	// change pin to different group
	
	void print();
	void print_output();
	void print_output_int();
};
#endif
