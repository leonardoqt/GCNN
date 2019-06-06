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
	std::vector <std::vector <int> > p_group; // to find which group the pin is in
public:
	int num_i;   // number of gates in input layer
	int num_ml;  // number of layers in middle layer
	int num_o;   // number of gates in output layer
	int num_tot; // total number of gates
	std::vector <int> num_acc; // accumulated number of gates in each layer
	std::vector <bool> input;
	std::vector <int> output;
	//=======function====================
	void init(int in, int middle_l, int out);
	// Init. rule:
	/*
	1.	linearly add middle layers
	*/
	
	void rand_connect();
	// Connect rule:
	/*
	01.	Two outputs cannot connect together
		(So can start with number of groups equal number of gates,
		each group start with one out, can they never change group.
		New group that contains purely in's can be created or deleted)
	02.	Inputs cannot connect to self outputs (can change)
	03.	May consider "always on" and "always off" group
	11.	Connect input pin to output pin in previous layer (may change to 
		all existing gates in previous layers)
	12.	second in-pin of input gate can connect to in-pin group, always on, or always off
	*/
	
	void count_group();
	void reset_gate();

	void run(int code);	// convert code to binary
	// Running rule:
	/*
	1.	Inputs cannot directly change outputs
	2.	One gate at most runs once
	3.	The second input of all in_gates are set to false (can change this rule)
	*/
	// Notes:
	/*
	1.	Consider allow out-gate to always run but if no non-out-gate can run then terminate
	2.	Consider if no gate can run, allow next non-run gate to send false from output pin
	*/

	void mutate();	// change pin to different group
	void mutate(double rate);	// change pin to different group with a rate
	
	void print();
	void print_output();
	void print_output_int();
};
#endif
