#include <stdlib.h>
#include "process.h"

Process::Process(bool CPU, int num_bursts, int id, int time)
{
	this->CPU_bound = CPU;
	if(CPU == true)
	{
		bursts_left = num_bursts;
	}
	randomize_CPU_burst();
	birthday = time;
	process_id = id;
}

// Chooses a random time in between 20-200 ms for interactive processes
// or a random time in between 20-3000 ms for CPU bound processes
void Process::randomize_CPU_burst()
{
	/* initialize random seed: */
 	 srand (time(NULL));
	if(!this->is_CPU())
	{
		//should be 2881, is 181 for testing purposes
		this->remaining_CPU_burst = rand() % 181 + 20;
	}
	else
	{
		this->remaining_CPU_burst = rand() % 181 + 20;
	}
}


// Chooses a random time in between 1000-4500 ms for interactive processes
// or a random time in between 1200-3200 ms for CPU bound processes
void Process::randomize_IO_burst()
{
	/* initialize random seed: */
 	 srand (time(NULL));
	if(!this->is_CPU())
	{
		this->remaining_IO = rand() % 3501 + 1000;
	}
	else
	{
		this->remaining_IO = rand() % 2001 + 1200;
	}
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