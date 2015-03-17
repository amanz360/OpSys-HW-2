#include <vector>
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <windows.h>

#include "cpu.h"
#include "process.h"

using namespace std;

vector<Process> generateProcesses(int num_processes, int num_bursts)
{
	vector<Process> process_list;
	
	int num_cpu = num_processes/5;
	
	for(int i = 0; i < num_processes; i++)
	{
		if(i < num_cpu)
		{
			process_list.push_back(Process(true, num_bursts, i+1, 0));
		}
		else
		{
			process_list.push_back(Process(false, -1, i+1, 0));
		}
	}
	
	return process_list;
}

/*bool cpuBoundDone(vector<Process> process_list)
{
	for(unsigned int i = 0; i < process_list.size(); i++)
	{
		if(process_list[i].is_CPU() && process_list[i].get_bursts() > 0)
		{
			return false;
		}	
	
	}
	
	return true;
}*/

//vector<int> shortestJobFirst(int num_processes, int num_bursts, int num_cpu);

int main(int argc, char** argv)
{

	//TODO: Write functions for each algorithm
	// To make the functions tunable, include parameters for number
	// of processes and number of CPU's. There could be more things
	// to make tunable.
	//int num_processes = 5, num_bursts = 6, num_cpu = 4;

	/*int num_processes = 12;
	int num_bursts = 6;
	int num_cpu = num_processes/5;
	srand(time(NULL));

	vector<Process> p = generateProcesses(num_processes, num_bursts); // vector of all processes
	for (unsigned int i=0; i<p.size(); i++) {
		cout << "process[" << p[i].get_process_ID() << "]\n";
		for (int j=0; j<num_bursts; j++) {
			cout << "   " << p[i].get_cpu_vec()[j] << "\n";
		}
	}*/
	
	//shortestJobFirst(num_processes, num_bursts, num_cpu);
}

/*vector<int> shortestJobFirst(int num_processes, int num_bursts, int num_CPU)
{
	//Prepare all of the initial conditions
	vector<Process> process_list = generateProcesses(num_processes, num_bursts);
	vector<Process> ready_queue = process_list;
	vector<Process> io_queue;
	vector<CPU> cpu_list;
	int min_turnaround = -1;
	int max_turnaround = 0;
	int average_turnaround = 0;
	int num_completions = 0;
	int time = 0;
	
	//Create the CPU list
	for(int i = 0; i < num_CPU; i++)
	{
		cpu_list.push_back(CPU());
	}
	
	//Print out initial entering of ready queue
	for(unsigned int i = 0; i < ready_queue.size(); i++)
	{
		if(ready_queue[i].is_CPU())
		{
				std::cout << "[time " << time << "ms] CPU-bound process ID " << ready_queue[i].get_process_ID() << " entered ready queue (requires " << ready_queue[i].get_CPU_time()
																																							<< "ms CPU 	time)" << std::endl;
		} 
		else
		{
			std::cout << "[time " << time << "ms] Interactive process ID " << ready_queue[i].get_process_ID() << " entered ready queue (requires " << ready_queue[i].get_CPU_time()
																																							<< "ms CPU 	time)" << std::endl;
		}
	}
	while(!cpuBoundDone(process_list))
	{
		//Remove any process that is done doing IO from the IO queue and place it into the ready queue
		for(unsigned int i = 0; i < io_queue.size(); i++)
		{
			if(io_queue[i].get_IO_time() == 0)
			{
				io_queue[i].randomize_CPU_burst();
				io_queue[i].reset();
				if(io_queue[i].is_CPU())
				{
					std::cout << "[time " << time << "ms] CPU-bound process ID " << io_queue[i].get_process_ID() << " entered ready queue (requires " << ready_queue[i].get_CPU_time()
																																							<< "ms CPU 	time)" << std::endl;
				} 
				else
				{
					std::cout << "[time " << time << "ms] Interactive process ID " << io_queue[i].get_process_ID() << " entered ready queue (requires " << ready_queue[i].get_CPU_time()
																																								<< "ms CPU 	time)" << std::endl;
				}
				
				ready_queue.push_back(io_queue[i]);
				io_queue.erase(io_queue.begin() + i);
			}
		}
		
		
		//Find any CPU that isn't in use and select the most appropriate process for it
		for(unsigned int i = 0; i < cpu_list.size(); i++)
		{
			if(cpu_list[i].in_use == false && ready_queue.size() > 0)
			{
				int min_time = 9999;
				int index = 0;
				for(unsigned int j = 0; j < ready_queue.size(); j++)
				{
					if(ready_queue[j].get_CPU_time() < min_time)
					{
						min_time = ready_queue[i].get_CPU_time();
						index = j;
					}
				}
				cpu_list[i].change_process(ready_queue[index], time);
				ready_queue.erase(ready_queue.begin() + index);
			}
		}
		
		//Run any CPU's that have a process
		for(unsigned int i = 0; i < cpu_list.size(); i++)
		{
			if(cpu_list[i].in_use == true)
			{
				//if a process completes, it will return true
				bool cpu_done = cpu_list[i].run_CPU(time);
				//if a process completes, we have to pick a new process for the CPU and put the previous process into its IO burst
				if(cpu_done)
				{
					Process* p = cpu_list[i].get_process();
					//if a process is CPU-bound and completes its required number of bursts, it is done and doesn't need to go into any other queues
					if(p->is_CPU() == true && p->get_bursts() == 0)
					{
						//wooo
					}
					else
					{
						p->randomize_IO_burst();
						io_queue.push_back(*p);
					}
					//If there is a process waiting for a CPU, pick the most appropriate one and give it to the current CPU
					if(ready_queue.size() > 0)
					{
						int min_time = 9999;
						int index = 0;
						for(unsigned int j = 0; j < ready_queue.size(); j++)
						{
							if(ready_queue[i].get_CPU_time() < min_time)
							{
								min_time = ready_queue[i].get_CPU_time();
								index = j;
							}
						}
						cpu_list[i].change_process(ready_queue[index], time);
						ready_queue.erase(ready_queue.begin() + index);
					}
					//Otherwise, if there is no process in the ready queue, simply remove association with the current process from the CPU
					else
					{
						cpu_list[i].remove_process();
					}
				}
				
			}

		}
		
		//Increase the wait time and turnaround time for any process in the ready queue
		for(unsigned int i = 0; i < ready_queue.size(); i++)
		{
			ready_queue[i].increment_wait();
			ready_queue[i].increment_turn();
		}
		//Increase the turnaround time for any process currently being run_CPU
		for(unsigned int i = 0; i < cpu_list.size(); i++)
		{
			if(cpu_list[i].in_use == true)
			{
				Process* p = cpu_list[i].get_process();
				p->increment_turn();
			}
		}
		//Run IO for all processes currently in IO stage
		for(unsigned int i = 0; i < io_queue.size(); i++)
		{
			io_queue[i].decrement_IO();
		}
		time++;
		
		Sleep(5);
	}
	
	vector<int> place_holder;
	
	return place_holder;

}*/

