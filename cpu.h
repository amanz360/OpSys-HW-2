#ifndef __CPU__
#define __CPU__

#include <vector>
#include <fstream>
#include "process.h"

using namespace std;

class CPU
{
	public:
		CPU();
		bool run_CPU(int time, ofstream &outstream);
		bool in_use;
		int time_used;
		void change_process(Process * p, int time, ofstream &outstream);
		void add_process(Process *p);
		void remove_process();
		Process* get_process();
		
	private:
		Process* current_process;
		int t_context;
		
};

#endif

