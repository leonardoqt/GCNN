#include <iostream>
#include <cstdlib>
#include "circuit.h"

using namespace std;

void circuit :: init(int in, int middle, int out)
{
	num_i = in;
	num_m = middle;
	num_o = out;
	num_tot = in+middle+out;
	list_g.resize(num_tot);
	group.resize(num_tot);
	l_group.resize(num_tot);
	for (auto& t1 : l_group)
		t1.resize(3);
	to_run.resize(0);
	output.resize(num_o);
}

void circuit :: rand_connect(double p_new)
{
	int gg;
	// Add each output to the beginning of each group
	for(size_t t1=0; t1!=num_tot; ++t1)
	{
		group[t1].resize(0);
		group[t1].push_back(2+3*t1);
	}
	// Bind all inputs to groups
	for(size_t t1=0; t1!=num_tot; ++t1)
	{
		// first pin
		// add to old group
		if (rand()/(double)RAND_MAX > p_new)
		{
			gg = rand()%(group.size()-1);
			if (gg == t1)
				gg++;
			group[gg].push_back(0+t1*3);
		}
		// create new group
		else
		{
			group.resize(group.size()+1);
			group[group.size()-1].resize(0);
			group[group.size()-1].push_back(0+t1*3);
		}
		// second pin
		// add to old group
		if (rand()/(double)RAND_MAX > p_new)
		{
			gg = rand()%(group.size()-1);
			if (gg == t1)
				gg++;
			group[gg].push_back(1+t1*3);
		}
		// create new group
		else
		{
			group.resize(group.size()+1);
			group[group.size()-1].resize(0);
			group[group.size()-1].push_back(1+t1*3);
		}
	}
}

void circuit :: count_group()
{
	for(size_t t1=0; t1!=group.size(); ++t1)
		for(size_t t2=0; t2!=group[t1].size(); ++t2)
			l_group[group[t1][t2]/3][group[t1][t2]%3] = t1;
}

void circuit :: run(bool *& input)
{
	int tmp_g, tmp_p;
	int min_run_g;
	int force_act_g;
	vector <int> tmp_run;

	to_run.resize(0);
	// read input
	for(size_t t1=0; t1!=num_i; t1++)
	{
		// read first pin
		for(auto& m2 : group[l_group[t1][0]])
		{
			tmp_g = m2/3;
			tmp_p = m2%3;
			if (tmp_p != 2)
			{
				list_g[tmp_g].p[tmp_p] = input[t1];
				list_g[tmp_g].if_p[tmp_p] = true;
				//cout<<tmp_g<<'\t'<<list_g[tmp_g].if_p[0]<<'\t'<<list_g[tmp_g].if_p[1]<<'\t'<<list_g[tmp_g].if_p[2]<<endl;
				if (list_g[tmp_g].if_p[0] && list_g[tmp_g].if_p[1] && !list_g[tmp_g].if_p[2])
				{
					//cout<<"Run"<<endl;
					to_run.push_back(tmp_g);
					if (tmp_g < num_i+num_m)
						list_g[tmp_g].if_p[2] = true;
				}
			}
		}
		// read second pin (for now all false)
		for(auto& m2 : group[l_group[t1][1]])
		{
			tmp_g = m2/3;
			tmp_p = m2%3;
			if (tmp_p != 2)
			{
				list_g[tmp_g].p[tmp_p] = false;	// current convention, second pin of in-gate always true
				list_g[tmp_g].if_p[tmp_p] = true;
				//cout<<tmp_g<<'\t'<<list_g[tmp_g].if_p[0]<<'\t'<<list_g[tmp_g].if_p[1]<<'\t'<<list_g[tmp_g].if_p[2]<<endl;
				if (list_g[tmp_g].if_p[0] && list_g[tmp_g].if_p[1] && !list_g[tmp_g].if_p[2])
				{
					//cout<<"Run"<<endl;
					to_run.push_back(tmp_g);
					if (tmp_g < num_i+num_m)
						list_g[tmp_g].if_p[2] = true;
				}
			}
		}
	}
	// find the smallest gate in to_run
	min_run_g = num_tot;
	for(auto& m1 : to_run)
		if (min_run_g > m1)
			min_run_g = m1;
	//cout<<"First chain run"<<endl;
	// start run
	while(to_run.size() && min_run_g < num_i+num_m)
	{
		while(to_run.size() && min_run_g < num_i+num_m)
		{
			tmp_run = to_run;
			to_run.resize(0);
			//cout<<"Start run"<<endl;
			for(auto& m1 : tmp_run)
			{
				list_g[m1].run();
				for(auto& m2 : group[l_group[m1][2]])
				{
					tmp_g = m2/3;
					tmp_p = m2%3;
					if (tmp_p != 2)
					{
						list_g[tmp_g].p[tmp_p] = list_g[m1].p[2];
						list_g[tmp_g].if_p[tmp_p] = true;
						//cout<<tmp_g<<'\t'<<list_g[tmp_g].if_p[0]<<'\t'<<list_g[tmp_g].if_p[1]<<'\t'<<list_g[tmp_g].if_p[2]<<endl;
						if (list_g[tmp_g].if_p[0] && list_g[tmp_g].if_p[1] && !list_g[tmp_g].if_p[2])
						{
							//cout<<"Run"<<endl;
							to_run.push_back(tmp_g);
							if (tmp_g < num_i+num_m)
								list_g[tmp_g].if_p[2] = true;
						}
					}
				}
			}
			//cout<<"In-loop chain finished"<<endl;
			// find the smallest gate in to_run
			min_run_g = num_tot;
			for(auto& m1 : to_run)
				if (min_run_g < m1)
					min_run_g = m1;
		}
		// if no run gate, activate next non-run gate's output pin
		for(force_act_g = 0; force_act_g!=num_tot; force_act_g++)
			if(!list_g[force_act_g].if_p[2])
				break;
		//cout<<force_act_g<<endl;
		if(force_act_g < num_i+num_m)
		{
			//cout<<"Activate gate "<<force_act_g<<endl;
			to_run.resize(0);
			for(auto& m2 : group[l_group[force_act_g][2]])
			{
				tmp_g = m2/3;
				tmp_p = m2%3;
				if (tmp_p != 2)
				{
					// send false to not activated group
					list_g[tmp_g].p[tmp_p] = false;
					list_g[tmp_g].if_p[tmp_p] = true;
					//cout<<tmp_g<<'\t'<<list_g[tmp_g].if_p[0]<<'\t'<<list_g[tmp_g].if_p[1]<<'\t'<<list_g[tmp_g].if_p[2]<<endl;
					if (list_g[tmp_g].if_p[0] && list_g[tmp_g].if_p[1] && !list_g[tmp_g].if_p[2])
					{
						//cout<<"Run"<<endl;
						to_run.push_back(tmp_g);
						if (tmp_g < num_i+num_m)
							list_g[tmp_g].if_p[2] = true;
					}
				}
			}
			// find the smallest gate in to_run
			min_run_g = num_tot;
			for(auto& m1 : to_run)
				if (min_run_g > m1)
					min_run_g = m1;
			//cout<<"Min gate in force run "<<min_run_g<<endl;
		}
	}
	// save result
	for(size_t t1=0; t1<num_o; t1++)
		if (list_g[num_i+num_m+t1].p[2])
			output[t1] = 1;
		else
			output[t1] = 0;
}

void circuit :: reset_gate()
{
	for(size_t t1=0; t1!=num_tot; ++t1)
		list_g[t1].clear();
}

void circuit :: mutate(double p_new)
{
	int tmp_g, tmp_p, tmp_ind, ind_group;
	// find where in the group the pin is to move
	for(size_t t1=0; t1!=6; t1++)
	{
		tmp_g = rand()%num_tot;
		tmp_p = rand()%2;
		ind_group = l_group[tmp_g][tmp_p];
		tmp_ind = 0;
		for(auto& m1 : group[ind_group])
		{
			if (m1 == tmp_g*3+tmp_p)
				break;
			tmp_ind++;
		}
		group[ind_group].erase(group[ind_group].begin() + tmp_ind);
		// delete the whole group if no member left
		if(group[ind_group].size() == 0)
			group.erase(group.begin() + ind_group);
		// find new place for pin
		if (rand()/(double)RAND_MAX > p_new)
		{
			int gg = rand()%(group.size()-1);
			if (gg == tmp_g)
				gg++;
			group[gg].push_back(tmp_p+tmp_g*3);
		}
		// create new group
		else
		{
			group.resize(group.size()+1);
			group[group.size()-1].resize(0);
			group[group.size()-1].push_back(tmp_p+tmp_g*3);
		}
		count_group();
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
		cout<<"Gate "<<t1<<'\t'<<l_group[t1][0]<<'\t'<<l_group[t1][1]<<'\t'<<l_group[t1][2]<<endl;
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
