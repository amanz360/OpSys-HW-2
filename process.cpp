#include <stdlib.h>
#include <vector>
#include "process.h"

Process::Process(bool CPU, int num_bursts, int id, int time)
{
	this->CPU_bound = CPU;
	if(CPU == true)
	{
		bursts_left = num_bursts;
	}
	birthday = time;
	process_id = id;

	// Randomize burst times
	this->remaining_CPU_burst = randomize_CPU_burst();
	this->remaining_IO = randomize_IO_burst();
	if (!this->is_CPU())
	{
		// could need up to 150 more bursts for io-bound processes	
		for (int i=0; i<151; i++)
		{
			cpu_burst_times.push_back(randomize_CPU_burst());
			io_burst_times.push_back(randomize_IO_burst());
		}
	}
	else
	{
		// only need 5 more bursts for cpu-bound processes
		for (int i=0; i<5; i++)
		{
			cpu_burst_times.push_back(randomize_CPU_burst());
			io_burst_times.push_back(randomize_IO_burst());
		}
	}
}

void Process::get_CPU_burst()
{
	cpu_burst_times.erase(cpu_burst_times.begin());
	this->remaining_CPU_burst = cpu_burst_times[0];
}

void Process::get_IO_burst()
{
	io_burst_times.erase(io_burst_times.begin());
	this->remaining_IO = io_burst_times[0];
}

// Chooses a random time in between 20-200 ms for interactive processes
// or a random time in between 20-3000 ms for CPU bound processes
int Process::randomize_CPU_burst()
{
	/* initialize random seed: */
	int r;
	if (!this->is_CPU())
	{
		// interactive process: range 20-200
		r = rand() % 181 + 20;
	}
	else
	{
		// CPU-bound process: range 200-3000
		r = rand() % 2881 + 200;
	}
	return r;
}


// Chooses a random time in between 1000-4500 ms for interactive processes
// or a random time in between 1200-3200 ms for CPU bound processes
int Process::randomize_IO_burst()
{
	/* initialize random seed: */
	// either process: range 1000-4500
	int r = rand() % 3501 + 1000;
	return r;
}

// Updates the average wait and turnaround time when a process completes
void Process::update_averages()
{
	avg_turnaround = ( (avg_turnaround * completed_bursts) + turnaround_time ) / (completed_bursts + 1);
	avg_wait = ( (avg_wait * completed_bursts) + wait_time) / (completed_bursts + 1);
	completed_bursts++;
}

// Resets the turnaround and wait time back to 0
void Process::reset()
{
	turnaround_time = 0;
	wait_time = 0;
}

// Returns the process' birthday
int Process::get_birthday() {return this->birthday;}

// Returns the remaining amount of time left for the current burst to complete
int Process::get_CPU_time() {return this->remaining_CPU_burst;}

// Returns the remaining amount of time left for the current IO to complete
int Process::get_IO_time(){return this->remaining_IO;}

// Returns whether or not this process is CPU bound
bool Process::is_CPU(){return this->CPU_bound;}

// Returns the number of bursts left for a CPU bound process
int Process::get_bursts(){return bursts_left;}

// Returns the total turnaround time for this burst
int Process::get_turn() {return turnaround_time;}

// Returns the total wait time for this burst
int Process::get_wait() {return wait_time;}

// Returns the average turnaround time
int Process::get_avg_turn(){return avg_turnaround;}

// Returns the average wait time
int Process::get_avg_wait(){return avg_wait;}

// Returns this process' ID
int Process::get_process_ID(){return this->process_id;}

// Increments the process' wait time by 1
void Process::increment_wait() {wait_time++;}

// Increments the process' turnaround time by 1000
void Process::increment_turn() {turnaround_time++;}

// Decrements the remaining CPU time by 1
void Process::decrement_CPU(){this->remaining_CPU_burst--;}

// Decrements the remaining CPU time by 1
void Process::decrement_IO(){this->remaining_IO--;}

// Decrements the number of bursts left by 1
void Process::decrement_bursts() {bursts_left--;}

// Returns vector of CPU burst times
std::vector<int> Process::get_cpu_vec() {return this->cpu_burst_times;}

// Returns vector of IO burst times
std::vector<int> Process::get_io_vec() {return this->io_burst_times;}

