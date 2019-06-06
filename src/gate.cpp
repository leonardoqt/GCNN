#include "gate.h"

void gate_nand :: clear()
{
	p[0] = p[1] = p[2] = false;
	if_p[0] = false;
	if_p[1] = false;
	if_p[2] = false;
}

bool gate_nand :: run()
{
	p[2] = !(p[0] && p[1]);
	return p[2];
}
