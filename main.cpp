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
		if(i <= num_cpu)
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

bool cpuBoundDone(vector<Process> process_list)
{
	for(unsigned int i = 0; i < process_list.size(); i++)
	{
		if(process_list[i].is_CPU() && process_list[i].get_bursts() > 0)
		{
			return false;
		}	
	
	}
	
	return true;
}

void firstComeFirstServe(vector<Process> p_list, int num_bursts, int num_cpu);
void roundRobin(vector<Process> p_list, int num_bursts, int num_CPU, int t_slice);

int main(int argc, char** argv)
{

	//TODO: Write functions for each algorithm
	// To make the functions tunable, include parameters for number
	// of processes and number of CPU's. There could be more things

	int num_processes = 12;
	int num_bursts = 6;
	int num_cpu = 4;
	int t_slice = 80;
	srand(time(NULL));
	

	vector<Process> p_list = generateProcesses(num_processes, num_bursts); // vector of all processes
	
	firstComeFirstServe(p_list, num_bursts, num_cpu);
	roundRobin(p_list, num_bursts, num_cpu, t_slice);
}
//         heheheh
//        vvvvv------------------get it hehhehehehheheogkdogkorgk
void firstComeFirstServe(vector<Process> p_list, int num_bursts, int num_CPU)
{
	vector<Process*> ready_queue;
	vector<Process*> io_queue;
	vector<CPU> cpu_list;
	int min_turn = -1, max_turn = 0, avg_turn = 0;
	int min_wait = -1, max_wait = 0, avg_wait = 0;
	int num_bursts_completed = 0;
	int time = 0;

	//Create the CPU list
	for(int i = 0; i < num_CPU; i++)
	{
		cpu_list.push_back(CPU());
	}
	
	//Print out initial entering of ready queue
	for(unsigned int i = 0; i < p_list.size(); i++)
	{	
		ready_queue.push_back(&p_list[i]);
		ready_queue[i]->reset();
		if(p_list[i].is_CPU())
		{
			std::cout << "[time " << time << "ms] CPU-bound process ID " << p_list[i].get_process_ID() << " entered ready queue (requires " << p_list[i].get_CPU_time()
																																				<< "ms CPU time)" << std::endl;
		} 
		else
		{
			std::cout << "[time " << time << "ms] Interactive process ID " << p_list[i].get_process_ID() << " entered ready queue (requires " << p_list[i].get_CPU_time()
																																				<< "ms CPU time)" << std::endl;
		}
	}

	while(!cpuBoundDone(p_list))
	{
		//Remove any process that is done doing IO from the IO queue and place it into the ready queue
		for(unsigned int i = 0; i < io_queue.size(); i++)
		{
			if(io_queue[i]->get_IO_time() == 0)
			{
				io_queue[i]->get_CPU_burst();
				//io_queue[i]->reset();
				if(io_queue[i]->is_CPU())
				{
					std::cout << "[time " << time << "ms] CPU-bound process ID " << io_queue[i]->get_process_ID() << " entered ready queue (requires " << io_queue[i]->get_CPU_time()
																																							<< "ms CPU time)" << std::endl;
				} 
				else
				{
					std::cout << "[time " << time << "ms] Interactive process ID " << io_queue[i]->get_process_ID() << " entered ready queue (requires " << io_queue[i]->get_CPU_time()
																																								<< "ms CPU time)" << std::endl;
				}
				
				ready_queue.push_back(io_queue[i]);
				ready_queue[ready_queue.size()-1]->reset();
				io_queue.erase(io_queue.begin() + i);
				i--;
			}
		}

		//Find any CPU that isn't in use and select the most appropriate process for it
		for(unsigned int i = 0; i < cpu_list.size(); i++)
		{
			if(cpu_list[i].in_use == false && ready_queue.size() > 0)
			{
				cpu_list[i].add_process(ready_queue[0]);
				ready_queue.erase(ready_queue.begin());
			}
		}

		//Run IO for all processes currently in IO stage
		for(unsigned int i = 0; i < io_queue.size(); i++)
		{
			io_queue[i]->decrement_IO();
		}

		//Run any CPU's that have a process
		for(unsigned int i = 0; i < cpu_list.size(); i++)
		{
			if(cpu_list[i].in_use == true)
			{
				//if a process completes, it will return true
				bool cpu_done = cpu_list[i].run_CPU(time);
				//if a process completes, we have to put the process into its IO burst
				if(cpu_done)
				{
					Process* p = cpu_list[i].get_process();
					// Update stats
					if (p->get_turn() > max_turn) {
						max_turn = p->get_turn();
					}
					if (min_turn == -1) {
						min_turn = p->get_turn();
					}
					if (min_turn >=0 && p->get_turn() < min_turn) {
						min_turn = p->get_turn();
					}
					if (p->get_wait() > max_wait) {
						max_wait = p->get_wait();
					}
					if (min_wait == -1) {
						min_wait = p->get_turn();
					}
					if (min_wait >= 0 && p->get_wait() < min_wait) {
						min_wait = p->get_wait();
					}
					avg_turn = ((avg_turn*num_bursts_completed) + p->get_turn())/(num_bursts_completed+1);
					avg_wait = ((avg_wait*num_bursts_completed) + p->get_wait())/(num_bursts_completed+1);
					num_bursts_completed++;

					//if a process is CPU-bound and completes its required number of bursts, it is done and doesn't need to go into any other queues
					if(p->is_CPU() == true && p->get_bursts() == 0)
					{
						//wooo
					}
					else
					{
						p->get_IO_burst();
						io_queue.push_back(p);
						cpu_list[i].remove_process();
					}
				}
			}
		}

		//Increase the wait time and turnaround time for any process in the ready queue
		for(unsigned int i = 0; i < ready_queue.size(); i++)
		{
			ready_queue[i]->increment_wait();
			ready_queue[i]->increment_turn();
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

		time++;
	}

	// Display stats
	std::cout << "\nTurnaround time: min " << min_turn << "; avg " << avg_turn << "ms; max " << max_turn << "ms\n";
	std::cout << "Total wait time: min " << min_wait << "; avg " << avg_wait << "ms; max " << max_wait << "ms\n";
	double answer = 0;
	for(unsigned int i = 0; i < cpu_list.size(); i++)
	{
		answer += cpu_list[i].time_used;
	}
	answer /= num_CPU;
	answer = (answer /(double) time) * 100;
	std::cout << "Average CPU utilization: " << answer << "%\n\n";

	std::cout << "Average CPU utilization per process:\n";	
	for (unsigned int i=0; i<p_list.size(); i++) {
		answer = ((double)p_list[i].get_use() / (double) time) * 100;
		answer /= num_CPU;
		std::cout << "process " << p_list[i].get_process_ID() << ": " << answer << "%\n";
	}

}

void roundRobin(vector<Process> p_list, int num_bursts, int num_CPU, int t_slice)
{
	vector<Process*> ready_queue;
	vector<Process*> io_queue;
	vector<CPU> cpu_list;
	int min_turn = -1, max_turn = 0, avg_turn = 0;
	int min_wait = -1, max_wait = 0, avg_wait = 0;
	int num_bursts_completed = 0;
	int time = 0;

	//Create the CPU list
	for(int i = 0; i < num_CPU; i++)
	{
		cpu_list.push_back(CPU());
	}
	
	//Print out initial entering of ready queue
	for(unsigned int i = 0; i < p_list.size(); i++)
	{	
		ready_queue.push_back(&p_list[i]);
		ready_queue[i]->reset();
		if(p_list[i].is_CPU())
		{
			std::cout << "[time " << time << "ms] CPU-bound process ID " << p_list[i].get_process_ID() << " entered ready queue (requires " << p_list[i].get_CPU_time()
																																				<< "ms CPU time)" << std::endl;
		} 
		else
		{
			std::cout << "[time " << time << "ms] Interactive process ID " << p_list[i].get_process_ID() << " entered ready queue (requires " << p_list[i].get_CPU_time()
																																				<< "ms CPU time)" << std::endl;
		}
	}

	while(!cpuBoundDone(p_list))
	{
		//Remove any process that is done doing IO from the IO queue and place it into the ready queue
		for(unsigned int i = 0; i < io_queue.size(); i++)
		{
			if(io_queue[i]->get_IO_time() == 0)
			{
				io_queue[i]->get_CPU_burst();
				//io_queue[i]->reset();
				if(io_queue[i]->is_CPU())
				{
					std::cout << "[time " << time << "ms] CPU-bound process ID " << io_queue[i]->get_process_ID() << " entered ready queue (requires " << io_queue[i]->get_CPU_time()
																																							<< "ms CPU time)" << std::endl;
				} 
				else
				{
					std::cout << "[time " << time << "ms] Interactive process ID " << io_queue[i]->get_process_ID() << " entered ready queue (requires " << io_queue[i]->get_CPU_time()
																																								<< "ms CPU time)" << std::endl;
				}
				
				ready_queue.push_back(io_queue[i]);
				ready_queue[ready_queue.size()-1]->reset();
				io_queue.erase(io_queue.begin() + i);
				i--;
			}
		}

		//Find any CPU that isn't in use and select the most appropriate process for it
		for(unsigned int i = 0; i < cpu_list.size(); i++)
		{
			if(cpu_list[i].in_use == false && ready_queue.size() > 0)
			{
				cpu_list[i].add_process(ready_queue[0]);
				ready_queue.erase(ready_queue.begin());
			}
		}

		//Run IO for all processes currently in IO stage
		for(unsigned int i = 0; i < io_queue.size(); i++)
		{
			io_queue[i]->decrement_IO();
		}

		//Run any CPU's that have a process
		for(unsigned int i = 0; i < cpu_list.size(); i++)
		{
			if(cpu_list[i].in_use == true)
			{
				//if a process completes, it will return true
				bool cpu_done = cpu_list[i].run_CPU(time);
				//if a process completes, we have to put the process into its IO burst
				if(cpu_done)
				{
					Process* p = cpu_list[i].get_process();
					// Update stats
					if (p->get_turn() > max_turn) {
						max_turn = p->get_turn();
					}
					if (min_turn == -1) {
						min_turn = p->get_turn();
					}
					if (min_turn >=0 && p->get_turn() < min_turn) {
						min_turn = p->get_turn();
					}
					if (p->get_wait() > max_wait) {
						max_wait = p->get_wait();
					}
					if (min_wait == -1) {
						min_wait = p->get_turn();
					}
					if (min_wait >= 0 && p->get_wait() < min_wait) {
						min_wait = p->get_wait();
					}
					avg_turn = ((avg_turn*num_bursts_completed) + p->get_turn())/(num_bursts_completed+1);
					avg_wait = ((avg_wait*num_bursts_completed) + p->get_wait())/(num_bursts_completed+1);
					num_bursts_completed++;

					//if a process is CPU-bound and completes its required number of bursts, it is done and doesn't need to go into any other queues
					if(p->is_CPU() == true && p->get_bursts() == 0)
					{
						//wooo
					}
					else
					{
						p->get_IO_burst();
						io_queue.push_back(p);
						cpu_list[i].remove_process();
					}
				}
				else if(cpu_list[i].get_process()->get_cur_burst() == t_slice)
				{
					if(ready_queue.size() > 0)
					{
						ready_queue.push_back(cpu_list[i].get_process());
						cpu_list[i].change_process(ready_queue[0], time);
						ready_queue.erase(ready_queue.begin());
					}
					else
					{
						cpu_list[i].change_process(cpu_list[i].get_process(), time);
					}
				}
				/*else
				{
					std::cout << "currently on tick " << cpu_list[i].get_process()->get_cur_burst() << " of current burst for process " << cpu_list[i].get_process()->get_process_ID() << std::endl;
				}*/
			}
		}

		//Increase the wait time and turnaround time for any process in the ready queue
		for(unsigned int i = 0; i < ready_queue.size(); i++)
		{
			ready_queue[i]->increment_wait();
			ready_queue[i]->increment_turn();
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

		time++;
	}

	// Display stats
	std::cout << "\nTurnaround time: min " << min_turn << "; avg " << avg_turn << "ms; max " << max_turn << "ms\n";
	std::cout << "Total wait time: min " << min_wait << "; avg " << avg_wait << "ms; max " << max_wait << "ms\n";
	double answer = 0;
	for(unsigned int i = 0; i < cpu_list.size(); i++)
	{
		answer += cpu_list[i].time_used;
	}
	answer /= num_CPU;
	answer = (answer /(double) time) * 100;
	std::cout << "Average CPU utilization: " << answer << "%\n\n";

	std::cout << "Average CPU utilization per process:\n";	
	for (unsigned int i=0; i<p_list.size(); i++) {
		answer = ((double)p_list[i].get_use() / (double) time) * 100;
		answer /= num_CPU;
		std::cout << "process " << p_list[i].get_process_ID() << ": " << answer << "%\n";
	}

}
