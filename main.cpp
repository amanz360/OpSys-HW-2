#include <vector>
#include <iostream>
#include <fstream>
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
			process_list.push_back(Process(false, num_bursts*60, i+1, 0));
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

void firstComeFirstServe(vector<Process> p_list, int num_bursts, int num_CPU, ofstream &FCFS_output);
void shortestJobFirst(vector<Process> p_list, int num_bursts, int num_CPU, ofstream &SJF_output);
void shortestRemainingTime(vector<Process> p_list, int num_bursts, int num_CPU, ofstream &SRT_output);
void roundRobin(vector<Process> p_list, int num_bursts, int num_CPU, int t_slice, ofstream &RR_output);

int main(int argc, char** argv)
{

	
	//Default values for tunable parameters
	int num_processes = 12;
	int num_bursts = 6;
	int num_cpu = 4;
	int t_slice = 80;
	srand(time(NULL));

	char input;
	std::cout << "Do you want to use the default values? (y/n): ";
	std::cin >> input;
	if(tolower(input) == 'n')
	{
		std::cout << "Please input the number of processes you want: ";
		std::cin >> num_processes;
		std::cout << "Please input the number of bursts per CPU process: ";
		std::cin >> num_bursts;
		std::cout << "Please input the number of CPU's: ";
		std::cin >> num_cpu;
		std::cout << "Please input the length of the time slice for RR: ";
		std::cin >> t_slice;
	}
	else
	{
		std::cout << "I'll take that as a yes." << std::endl;
	}
	std::cout << "num processes: " << num_processes << " / num_bursts: " << num_bursts << " / num_cpu: " << num_cpu << " / t_slice: " << t_slice << std::endl;
	

	vector<Process> p_list = generateProcesses(num_processes, num_bursts); // vector of all processes

	ofstream FCFS_output, SJF_output, SRT_output, RR_output;
  	FCFS_output.open ("FCFS_log", ios::trunc);
  	SJF_output.open ("SJF_log", ios::trunc);
  	SRT_output.open ("SRT_log", ios::trunc);
  	RR_output.open ("RR_log", ios::trunc);
	
	std::cout << "===============================================\n";
	std::cout << "\tFIRST COME FIRST SERVE\n";
	firstComeFirstServe(p_list, num_bursts, num_cpu, FCFS_output);
	std::cout << "===============================================\n";
	std::cout << "\tSHORTEST JOB FIRST\n";
	shortestJobFirst(p_list, num_bursts, num_cpu, SJF_output);
	std::cout << "===============================================\n";
	std::cout << "\tSHORTEST REMAINING TIME\n";
	shortestRemainingTime(p_list, num_bursts, num_cpu, SRT_output);
	std::cout << "===============================================\n";
	std::cout << "\tROUND ROBIN\n";
	roundRobin(p_list, num_bursts, num_cpu, t_slice, RR_output);
	std::cout << "===============================================\n";

}
//         heheheh
//        vvvvv------------------get it hehhehehehheheogkdogkorgk
void firstComeFirstServe(vector<Process> p_list, int num_bursts, int num_CPU, ofstream &FCFS_output)
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
			FCFS_output << "[time " << time << "ms] CPU-bound process ID " << p_list[i].get_process_ID() << " entered ready queue (requires " << p_list[i].get_CPU_time()
																																				<< "ms CPU time)" << std::endl;
		} 
		else
		{
			FCFS_output << "[time " << time << "ms] Interactive process ID " << p_list[i].get_process_ID() << " entered ready queue (requires " << p_list[i].get_CPU_time()
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
					FCFS_output << "[time " << time << "ms] CPU-bound process ID " << io_queue[i]->get_process_ID() << " entered ready queue (requires " << io_queue[i]->get_CPU_time()
																																							<< "ms CPU time)" << std::endl;
				} 
				else
				{
					FCFS_output << "[time " << time << "ms] Interactive process ID " << io_queue[i]->get_process_ID() << " entered ready queue (requires " << io_queue[i]->get_CPU_time()
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
				bool cpu_done = cpu_list[i].run_CPU(time, FCFS_output);
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


void shortestJobFirst(vector<Process> p_list, int num_bursts, int num_CPU, ofstream &SJF_output)
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
	unsigned int index; // 
	for(unsigned int i = 0; i < p_list.size(); i++)
	{
		// Order processes in queue by shortest job first
		index = ready_queue.size();
		for (unsigned int j=0; j<ready_queue.size(); j++)
		{
			if (p_list[i].get_CPU_time() < ready_queue[j]->get_CPU_time())
			{
				index = j;
				break;
			}
		}
		// insert into ready queue at index
		ready_queue.insert(ready_queue.begin()+index,&p_list[i]);
		ready_queue[index]->reset();
		if(p_list[i].is_CPU())
		{
			SJF_output << "[time " << time << "ms] CPU-bound process ID " << p_list[i].get_process_ID() << " entered ready queue (requires " << p_list[i].get_CPU_time()
																																							<< "ms CPU time)" << std::endl;
		} 
		else
		{
			SJF_output << "[time " << time << "ms] Interactive process ID " << p_list[i].get_process_ID() << " entered ready queue (requires " << p_list[i].get_CPU_time()
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
					SJF_output << "[time " << time << "ms] CPU-bound process ID " << io_queue[i]->get_process_ID() << " entered ready queue (requires " << io_queue[i]->get_CPU_time()
																																							<< "ms CPU time)" << std::endl;
				} 
				else
				{
					SJF_output << "[time " << time << "ms] Interactive process ID " << io_queue[i]->get_process_ID() << " entered ready queue (requires " << io_queue[i]->get_CPU_time()
																																								<< "ms CPU time)" << std::endl;
				}
				// Find index to put process in ready_queue in order of shortest job first
				index = ready_queue.size();
				for (unsigned int j=0; j<ready_queue.size(); j++)
				{
					if (p_list[i].get_CPU_time() < ready_queue[j]->get_CPU_time())
					{
						index = j;
						break;
					}
				}
				ready_queue.insert(ready_queue.begin()+index,io_queue[i]);
				ready_queue[index]->reset();
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
				bool cpu_done = cpu_list[i].run_CPU(time, SJF_output);
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

void shortestRemainingTime(vector<Process> p_list, int num_bursts, int num_CPU, ofstream &SRT_output)
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
	unsigned int index;
	for(unsigned int i = 0; i < p_list.size(); i++)
	{
		// Order processes in queue by shortest job first
		index = ready_queue.size();
		for (unsigned int j=0; j<ready_queue.size(); j++)
		{
			if (p_list[i].get_CPU_time() < ready_queue[j]->get_CPU_time())
			{
				index = j;
				break;
			}
		}
		// insert into ready queue at index
		ready_queue.insert(ready_queue.begin()+index,&p_list[i]);
		ready_queue[index]->reset();
		if(p_list[i].is_CPU())
		{
			SRT_output << "[time " << time << "ms] CPU-bound process ID " << p_list[i].get_process_ID() << " entered ready queue (requires " << p_list[i].get_CPU_time()
																																							<< "ms CPU time)" << std::endl;
		} 
		else
		{
			SRT_output << "[time " << time << "ms] Interactive process ID " << p_list[i].get_process_ID() << " entered ready queue (requires " << p_list[i].get_CPU_time()
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
					SRT_output << "[time " << time << "ms] CPU-bound process ID " << io_queue[i]->get_process_ID() << " entered ready queue (requires " << io_queue[i]->get_CPU_time()
																																							<< "ms CPU time)" << std::endl;
				} 
				else
				{
					SRT_output << "[time " << time << "ms] Interactive process ID " << io_queue[i]->get_process_ID() << " entered ready queue (requires " << io_queue[i]->get_CPU_time()
																																								<< "ms CPU time)" << std::endl;
				}
				// Find index to put process in ready_queue in order of shortest job first
				index = ready_queue.size();
				for (unsigned int j=0; j<ready_queue.size(); j++)
				{
					if (p_list[i].get_CPU_time() < ready_queue[j]->get_CPU_time())
					{
						index = j;
						break;
					}
				}
				ready_queue.insert(ready_queue.begin()+index,io_queue[i]);
				ready_queue[index]->reset();
				io_queue.erase(io_queue.begin() + i);
				i--;
			}
		}

		//Find any CPU that isn't in use and select the most appropriate process for it,
		// And for each CPU in use, check if there is a shorter process in the ready queue
		for(unsigned int i = 0; i < cpu_list.size(); i++)
		{
			if(cpu_list[i].in_use == false && ready_queue.size() > 0)
			{
				cpu_list[i].add_process(ready_queue[0]);
				ready_queue.erase(ready_queue.begin());
			}
			else if(cpu_list[i].in_use && ready_queue.size() > 0)
			{
				if (cpu_list[i].get_process()->get_CPU_time() > ready_queue[0]->get_CPU_time())
				{
					// context switch: preempt process with shorter burst time
					index = ready_queue.size();
					for (unsigned int j=0; j<ready_queue.size(); j++)
					{
						if (cpu_list[i].get_process()->get_CPU_time() < ready_queue[j]->get_CPU_time())
						{
							index = j;
							break;
						}
					}
					ready_queue.insert(ready_queue.begin()+index,cpu_list[i].get_process());
					cpu_list[i].change_process(ready_queue[0],time, SRT_output);
					ready_queue.erase(ready_queue.begin());
				}
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
				bool cpu_done = cpu_list[i].run_CPU(time, SRT_output);
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

void roundRobin(vector<Process> p_list, int num_bursts, int num_CPU, int t_slice, ofstream &RR_output)
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
			RR_output << "[time " << time << "ms] CPU-bound process ID " << p_list[i].get_process_ID() << " entered ready queue (requires " << p_list[i].get_CPU_time()
																																				<< "ms CPU time)" << std::endl;
		} 
		else
		{
			RR_output << "[time " << time << "ms] Interactive process ID " << p_list[i].get_process_ID() << " entered ready queue (requires " << p_list[i].get_CPU_time()
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
					RR_output << "[time " << time << "ms] CPU-bound process ID " << io_queue[i]->get_process_ID() << " entered ready queue (requires " << io_queue[i]->get_CPU_time()
																																							<< "ms CPU time)" << std::endl;
				} 
				else
				{
					RR_output << "[time " << time << "ms] Interactive process ID " << io_queue[i]->get_process_ID() << " entered ready queue (requires " << io_queue[i]->get_CPU_time()
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
				bool cpu_done = cpu_list[i].run_CPU(time, RR_output);
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
				else if(cpu_list[i].get_process()->get_cur_burst() > t_slice)
				{
					if(ready_queue.size() > 0)
					{
						ready_queue.push_back(cpu_list[i].get_process());
						cpu_list[i].change_process(ready_queue[0], time, RR_output);
						ready_queue.erase(ready_queue.begin());
					}
					else
					{
						cpu_list[i].change_process(cpu_list[i].get_process(), time, RR_output);
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
