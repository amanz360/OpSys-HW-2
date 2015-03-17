#ifndef __CPU__
#define __CPU__

#include <vector>
#include "process.h"

using namespace std;

class CPU
{
	public:
		CPU();
		bool run_CPU(int time);
		bool in_use;
		int time_used;
		void add_process(Process & p, int time, int old_ID);
		void remove_process();
		Process* get_process();
		
	private:
		Process* current_process;
		int t_context;
		
};

#endif

