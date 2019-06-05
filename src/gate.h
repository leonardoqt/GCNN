#ifndef __GATE__
#define __GATE__

class circuit;
class gate_nand
{
private:
	bool p[3];
	bool if_p[3];
friend circuit;
public:
	void clear();
	bool run();
};

#endif
