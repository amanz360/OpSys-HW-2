


class Process
{

	public:
		Process(bool CPU, int num_bursts, int id, int time);
		void randomize_CPU_burst();
		void randomize_IO_burst();
		void increment_wait();
		void increment_turn();
		void decrement_CPU();
		void decrement_IO();
		void decrement_bursts();
		void update_averages();
		void reset();
		int get_CPU_time();
		int get_IO_time();
		int get_process_ID();
		int get_birthday();
		int get_bursts();
		int get_turn();
		int get_wait();
		int get_avg_turn();
		int get_avg_wait();
		bool is_CPU();
		
	private:
	
		int process_id;				//ID for process
		int birthday;				//Time at which the process was born
		int remaining_CPU_burst;	//Time remaining for current CPU burst to complete
		int remaining_IO;			//Time remaining for current IO to complete
		int bursts_left;			//Number of bursts left for completion(Only for CPU bound)
		int completed_bursts;		//Number of bursts that have been completed
		int avg_wait;				//Average wait time
		int avg_turnaround;			//Average turnaround time
		int wait_time;				//Time spent in ready queue
		int turnaround_time;		//Total time since entering ready queue
		bool CPU_bound;				//Indicates if it is CPU bound or interactive
			
};