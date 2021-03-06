#include <iostream>
#include <cstdlib>

#include "cpu.h"

CPU::CPU()
{
	time_used = 0;
	current_process = NULL;
	t_context = 0;
	in_use = false;
}

// Decrements the cpu burst time for the current process and checks
// if the time is equal to 0. If so, then it returns true, otherwise
// returns false. Also prints out burst completion.
bool CPU::run_CPU(int time, ofstream &outstream)
{
	if(t_context > 0)
	{
		t_context--;
		return false;
	}
	else
	{
		time_used++;
		(*current_process).increment_use();
		(*current_process).decrement_CPU();
		(*current_process).increment_cur_burst();
		if((*current_process).get_CPU_time() == 0)
		{
			current_process->update_averages();
			if((*current_process).is_CPU())
			{
				(*current_process).decrement_bursts();
				if((*current_process).get_bursts() == 0)
				{
					outstream << "[time " << time << "ms] CPU-bound process ID " << (*current_process).get_process_ID() << " terminated (avg turnaround time "<< (*current_process).get_avg_turn() << "ms, avg total wait time " << (*current_process).get_avg_wait() << "ms)" << std::endl;
				}
				else
				{
					outstream << "[time " << time << "ms] CPU-bound process ID " << (*current_process).get_process_ID() << " CPU burst done (turnaround time "<< (*current_process).get_turn() << "ms, total wait time " << (*current_process).get_wait() << "ms)" << std::endl;
				}
			}
			else
			{
				outstream << "[time " << time << "ms] Interactive process ID " << (*current_process).get_process_ID() << " CPU burst done (turnaround time "<< (*current_process).get_turn() << "ms, total wait time " << (*current_process).get_wait() << "ms)" << std::endl;
			}
			return true;
		}
	}
	return false;
}

Process* CPU::get_process()
{
	return current_process;
}

void CPU::remove_process()
{
	current_process = NULL;
	in_use = false;
}

void CPU::add_process(Process * p)
{
	current_process = p;
	in_use = true;
}

void CPU::change_process(Process * p, int time, ofstream &outstream)
{
	t_context += 4;
	outstream << "[time " << time << "ms] Context switch (swapping out process ID " << current_process->get_process_ID() << " to process ID " << p->get_process_ID() << ")" << std::endl;

	current_process->reset_burst();
	current_process = p;
	in_use = true;
}

