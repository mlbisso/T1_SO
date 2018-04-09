#include <stdint.h>

#pragma once

/****************************************************************************/
/*                                Tipos                                     */
/****************************************************************************/

struct burst;
typedef struct burst Burst;

struct burst
{
  int time;
  Burst* next_burst;
};


struct bursts;
typedef struct bursts Bursts;

struct bursts
{
	Burst* head;
	Burst* tail;
	int count;
};

struct process;
typedef struct process Process;

struct process
{
  int PID;
  char name[256];
  int state;     //0 NEW; 1 READY; 2 RUNNING; 3 FINISHED
  int init_time;
  int processed_time;  //para ver si superó quantum
  Bursts* bursts;
  Process* next_new;
  Process* previous_new;
  Process* next_process;
  Process* next_finished;
  int number_CPU;
  int number_interruptions;
  int turnaround_time;
  int response_time;
  int waiting_time;
  int in_cpu;
};

struct queue;
typedef struct queue Queue;

struct queue
{
	int state; //0 NEW; 1 READY; 2 RUNNING; 3 FINISHED
	Process* head;
	Process* tail;
	int count;
	int priority;
};


// /****************************************************************************/
// /*                               Funciones                                  */
// /****************************************************************************/
//
Process* process_init(int PID);
Burst* burst_init();
Bursts* bursts_init();
Queue* queue_init(int state, int priority);

void burst_insert(Bursts* bursts, Burst* burst);
void new_queue_insert(Queue* queue, Process* process);
void queues_insert(Queue* queue, Process* process);
void remove_burst(Process* process);
void remove_process(Queue* queue);
void move_process(Queue* queue);
void insert_process(Queue* queue, Process* process);
void finished_queue_insert(Queue* queue, Process* process);
void finished_remove_process(Queue* queue);
Process* get_process(Queue* queue, int actual_PID);
void recorrer_ready(Queue* queue);

int reading_file(char* file, Queue* new_queue, int total_processes);

Queue* unir_queues(Queue* menor, Queue* mayor);

int revision_finished(Queue* finished, Process* process);