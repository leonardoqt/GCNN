#include <iostream>
#include <cstdlib>
#include <cmath>
#include "circuit.h"

using namespace std;

void circuit :: init(int in, int ml, int out)
{
	num_i = in;
	num_ml = ml;
	num_o = out;
	num_acc.resize(ml+3);	//+3 for head, in, out
	num_acc[0] = 0;
	num_acc[1] = in;
	for(size_t t1=2; t1!=ml+2; ++t1)
		num_acc[t1] = num_acc[t1-1] + ceil((double)(out-in)*(t1-1)/(ml+1) + in);
	num_acc[ml+2] = num_acc[ml+1] + out;
	num_tot = num_acc[ml+2];
	list_g.resize(num_tot);
	group.resize(num_tot+in+2);	// default group: all out-pin and 1st in-pin in input layer, next two are all true, all false
	p_group.resize(num_tot);
	for (auto& m1 : p_group)
		m1.resize(3);
	to_run.resize(0);
	input.resize(num_i);
	output.resize(num_o);
}

void circuit :: rand_connect()
{
	int gg, ind_layer;
	// Add each output to the beginning of each group
	for(size_t t1=0; t1!=num_tot; ++t1)
	{
		group[t1].resize(0);
		group[t1].push_back(2+3*t1);
	}
	// Add 1st in-pin from input gates to the beginning of each group
	for(size_t t1=0; t1!=num_i; t1++)
	{
		group[t1+num_tot].resize(0);
		group[t1+num_tot].push_back(0+3*t1);
	}
	// Add 2nd in-pin from input gates to either in-pin group, all true, or all false
	group[num_tot+num_i].resize(0);
	group[num_tot+num_i+1].resize(0);
	for(size_t t1=0; t1!=num_i; t1++)
	{
		gg = rand()%3;
		switch (gg)
		{
		// add to in-pin group
		case 0:
		{
			gg = rand()%num_i + num_tot;
			group[gg].push_back(1+t1*3);
			break;
		}
		// add to all true group
		case 1:
		{
			gg = num_i+num_tot;
			group[gg].push_back(1+t1*3);
			break;
		}
		// add to all false group
		case 2:
		{
			gg = num_i+num_tot+1;
			group[gg].push_back(1+t1*3);
			break;
		}
		}
	}
	// Bind all in-pin from later layers to groups
	for(size_t t1=num_i; t1!=num_tot; ++t1)
	{
		// find layer
		for(size_t t2=0; t2!=num_acc.size(); ++t2)
			if (t1 < num_acc[t2+1])
			{
				ind_layer = t2;
				break;
			}
		// for different pin
		for(size_t t2=0; t2!=2; ++t2)
		{
			gg = rand()%(num_acc[ind_layer]-num_acc[ind_layer-1]) + num_acc[ind_layer-1];
			group[gg].push_back(t2+t1*3);
		}
	}
}

void circuit :: count_group()
{
	for(size_t t1=0; t1!=group.size(); ++t1)
		for(size_t t2=0; t2!=group[t1].size(); ++t2)
			p_group[group[t1][t2]/3][group[t1][t2]%3] = t1;
}

void circuit :: run(int code)
{
	int tmp_g, tmp_p;
	vector <int> tmp_run;
	int guard;

	to_run.resize(0);
	// convert code to input
	guard=1<<num_i;
	code = code%guard;
	for(size_t t1=0; t1!=num_i; ++t1)
	{
		guard/=2;
		input[t1] = (bool)(code/guard);
		code = code%guard;
	}
	// read input
	for(size_t t1=0; t1!=num_i; t1++)
	{
		// read first pin
		for(auto& m2 : group[p_group[t1][0]])
		{
			tmp_g = m2/3;
			tmp_p = m2%3;
			if (tmp_p != 2)
			{
				list_g[tmp_g].p[tmp_p] = input[t1];
				list_g[tmp_g].if_p[tmp_p] = true;
				if (list_g[tmp_g].if_p[0] && list_g[tmp_g].if_p[1] && !list_g[tmp_g].if_p[2])
				{
					to_run.push_back(tmp_g);
					list_g[tmp_g].if_p[2] = true;
				}
			}
		}
		// read group all true
		for(auto& m2 : group[num_tot+num_i])
		{
			tmp_g = m2/3;
			tmp_p = m2%3;
			if (tmp_p != 2)
			{
				list_g[tmp_g].p[tmp_p] = true;
				list_g[tmp_g].if_p[tmp_p] = true;
				if (list_g[tmp_g].if_p[0] && list_g[tmp_g].if_p[1] && !list_g[tmp_g].if_p[2])
				{
					to_run.push_back(tmp_g);
					list_g[tmp_g].if_p[2] = true;
				}
			}
		}
		// read group all false
		for(auto& m2 : group[num_tot+num_i+1])
		{
			tmp_g = m2/3;
			tmp_p = m2%3;
			if (tmp_p != 2)
			{
				list_g[tmp_g].p[tmp_p] = false;
				list_g[tmp_g].if_p[tmp_p] = true;
				if (list_g[tmp_g].if_p[0] && list_g[tmp_g].if_p[1] && !list_g[tmp_g].if_p[2])
				{
					to_run.push_back(tmp_g);
					list_g[tmp_g].if_p[2] = true;
				}
			}
		}
	}
	// start run
	while(to_run.size())
	{
		tmp_run = to_run;
		to_run.resize(0);
		for(auto& m1 : tmp_run)
		{
			list_g[m1].run();
			for(auto& m2 : group[p_group[m1][2]])
			{
				tmp_g = m2/3;
				tmp_p = m2%3;
				if (tmp_p != 2)
				{
					list_g[tmp_g].p[tmp_p] = list_g[m1].p[2];
					list_g[tmp_g].if_p[tmp_p] = true;
					if (list_g[tmp_g].if_p[0] && list_g[tmp_g].if_p[1] && !list_g[tmp_g].if_p[2])
					{
						to_run.push_back(tmp_g);
						list_g[tmp_g].if_p[2] = true;
					}
					//cout<<"Gate "<<m1<<" send "<<list_g[m1].p[2]<<" from "<<list_g[m1].p[0]<<' '<<list_g[m1].p[1]<<" to gate "<<tmp_g<<'-'<<tmp_p<<endl;
				}
			}
		}
	}
	// save result
	for(size_t t1=0; t1<num_o; t1++)
		if (list_g[num_acc[num_ml+1]+t1].p[2])
			output[t1] = 1;
		else
			output[t1] = 0;
}

void circuit :: reset_gate()
{
	for(size_t t1=0; t1!=num_tot; ++t1)
		list_g[t1].clear();
}

void circuit :: mutate()
{
	int tmp_g, tmp_p, ind_in_group, ind_group, ind_layer, gg;
	// find where in the group the pin is to move
	tmp_g = rand()%num_tot;
	if (tmp_g >= num_i)
		tmp_p = rand()%2;
	else
		tmp_p = 1;
	ind_group = p_group[tmp_g][tmp_p];
	ind_in_group = 0;
	for(auto& m1 : group[ind_group])
	{
		if (m1 == tmp_g*3+tmp_p)
			break;
		ind_in_group++;
	}
	group[ind_group].erase(group[ind_group].begin() + ind_in_group);
	// find new place for pin
	if (tmp_g >= num_i)
	{
		// find layer
		for(size_t t1=0; t1!=num_acc.size(); ++t1)
			if (tmp_g < num_acc[t1+1])
			{
				ind_layer = t1;
				break;
			}
		gg = rand()%(num_acc[ind_layer]-num_acc[ind_layer-1]) + num_acc[ind_layer-1];
	}
	else
	{
		gg = rand()%3;
		switch (gg)
		{
		// add to in-pin group
		case 0:
		{
			gg = rand()%num_i + num_tot;
			break;
		}
		// add to all true group
		case 1:
		{
			gg = num_i+num_tot;
			break;
		}
		// add to all false group
		case 2:
		{
			gg = num_i+num_tot+1;
			break;
		}
		}
	}
	group[gg].push_back(tmp_p+tmp_g*3);
	p_group[tmp_g][tmp_p] = gg;
}

void circuit :: mutate(double rate)
{
	int ind_group, ind_in_group, gg;
	for(size_t ind_layer = 1; ind_layer!=num_ml+2; ++ind_layer)
	for(size_t tmp_g = num_acc[ind_layer]; tmp_g!=num_acc[ind_layer+1]; ++tmp_g)
	for(size_t tmp_p = 0; tmp_p!=2; ++tmp_p)
	if((double)rand()/RAND_MAX < rate)
	{
		ind_group = p_group[tmp_g][tmp_p];
		ind_in_group = 0;
		for(auto& m1 : group[ind_group])
		{
			if (m1 == tmp_g*3+tmp_p)
				break;
			ind_in_group++;
		}
		group[ind_group].erase(group[ind_group].begin() + ind_in_group);
		gg = rand()%(num_acc[ind_layer]-num_acc[ind_layer-1]) + num_acc[ind_layer-1];
		group[gg].push_back(tmp_p+tmp_g*3);
		p_group[tmp_g][tmp_p] = gg;
		//cout<<"Move pin "<<tmp_g<<'-'<<tmp_p<<" from group "<<ind_group<<" to group "<<gg<<endl;
	}
	// for input layer
	for(size_t tmp_g = 0; tmp_g!=num_i; ++tmp_g)
	if((double)rand()/RAND_MAX < rate)
	{
		int tmp_p = 1;
		ind_group = p_group[tmp_g][tmp_p];
		ind_in_group = 0;
		for(auto& m1 : group[ind_group])
		{
			if (m1 == tmp_g*3+tmp_p)
				break;
			ind_in_group++;
		}
		group[ind_group].erase(group[ind_group].begin() + ind_in_group);
		gg = rand()%3;
		switch (gg)
		{
		// add to in-pin group
		case 0:
		{
			gg = rand()%num_i + num_tot;
			break;
		}
		// add to all true group
		case 1:
		{
			gg = num_i+num_tot;
			break;
		}
		// add to all false group
		case 2:
		{
			gg = num_i+num_tot+1;
			break;
		}
		}
		group[gg].push_back(tmp_p+tmp_g*3);
		p_group[tmp_g][tmp_p] = gg;
		//cout<<"Move pin "<<tmp_g<<'-'<<tmp_p<<" from group "<<ind_group<<" to group "<<gg<<endl;
	}
}

void circuit :: print()
{
	for(size_t t1=0; t1!=group.size(); ++t1)
	{
		cout<<"Group "<<t1<<endl;
		for(auto& t2 : group[t1])
			cout<<'\t'<<t2/3<<'-'<<t2%3;
		cout<<endl;
	}
	cout<<endl;
	for(size_t t1=0; t1!=num_tot; ++t1)
		cout<<"Gate "<<t1<<'\t'<<p_group[t1][0]<<'\t'<<p_group[t1][1]<<'\t'<<p_group[t1][2]<<endl;
}

void circuit :: print_output()
{
	cout<<"Results :"<<endl;
	for(auto& m1 : output)
		cout<<'\t'<<m1;
	cout<<endl;
}

void circuit :: print_output_int()
{
	int res = output[0];
	for(size_t t1=1; t1!=num_o; t1++)
		res = 2*res + output[t1];
	cout<<res<<endl;
}
