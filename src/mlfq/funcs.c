// Importamos la librería estándar de C
#include <stdlib.h>
// Importamos el módulo estándar de I/O
#include <stdio.h>
// Importamos las definiciones de la lista
#include "funcs.h"
#include <math.h>
#include <unistd.h>

#include <string.h>

Process* process_init(int PID)
{
	Process* process = malloc(sizeof(Process));
	process->bursts = bursts_init();
	process->PID = PID;
	process->state = 0; //NEW
	process->next_new = NULL;
	process->previous_new = NULL;
	process->next_process = NULL;
	process->next_finished = NULL;
	process->processed_time = 0;
	process->number_CPU = 0;
	process->number_interruptions = 0;
	process->turnaround_time = -1;
    process->response_time = -1;
    process->waiting_time = 0;
    process->in_cpu = 0;
	return process;
}

Queue* queue_init(int state, int priority)
{
	Queue* queue = malloc(sizeof(Queue));
	queue->state = state; //0 NEW; 1 READY; 2 RUNNING; 3 FINISHED
	queue -> count = 0;
	queue -> head = NULL;
	queue -> tail = NULL;
	queue -> priority = priority;	
	return queue;
}

Burst* burst_init(int time){
	Burst* burst = malloc(sizeof(Burst));
	burst->time = time;
	burst->next_burst = NULL;
	return burst;
}


Bursts* bursts_init()
{
	Bursts* bursts = malloc(sizeof(Bursts));
	bursts -> count = 0;
	bursts -> head = NULL;
	bursts -> tail = NULL;
	return bursts;
}
//
void burst_insert(Bursts* bursts, Burst* new){
	if(bursts -> count == 0){
		bursts -> tail = new;
		bursts -> head = new;
	}
	else{
		bursts -> tail -> next_burst = new;            //el siguiente del ultimo actual es new
		bursts -> tail = new;                                //se agrega new al final
		bursts -> tail -> next_burst = NULL;
	}
	bursts -> count++;	
}

void remove_burst(Process*process){
	process->bursts->head = process->bursts->head->next_burst;
	process->bursts->count -= 1;
}

void remove_process(Queue* queue){
	queue->  head = queue -> head -> next_process;
	queue -> count -= 1;
}

void finished_remove_process(Queue* queue){
	queue->  head = queue -> head -> next_finished;
	queue -> count -= 1;
}


void move_process(Queue* queue){
	if (queue->count > 1){
		Process* moved_process = queue -> head;
		queue-> head = queue -> head -> next_process;
		queue-> tail -> next_process = moved_process;
		queue-> tail = moved_process;
		queue -> tail -> next_process = NULL;
	}
}

void insert_process(Queue* queue, Process* process){
	if(queue -> count == 0)
	{
		queue -> tail = process;
		queue -> head = process;
	}
	else
	{
		queue -> tail -> next_process = process;      
		queue -> tail = process;                               
		queue -> tail -> next_process= NULL;
	}
	queue -> count++;
}

void recorrer_ready(Queue* queue){
	Process* actual = queue->head;
	while (actual != NULL){
		if (actual->state == 1){   //esta en ready
			actual->waiting_time += 1;
		}
		actual = actual->next_process;
	}
}

Process* get_process(Queue* queue, int actual_PID){
	Process* return_process = queue->head;
	while (return_process != NULL && return_process -> PID != actual_PID){
		return_process = return_process->next_process;
	}
	return return_process;
}


void new_queue_insert(Queue* queue, Process* process){
	if(queue -> count == 0){
		queue -> tail = process;
		queue -> head = process;
	}
	else if (process->init_time < queue->head->init_time){   //si hay que poner new a la cabeza por menor tiempo
		process -> next_new = queue -> head;
		queue -> head -> previous_new = process;
		queue -> head = process;
	}
	else{

		Process* actual = queue->head;
		Process* previous = NULL;
		while (actual && actual -> init_time <= process->init_time){
			previous = actual;
			actual = actual-> next_new;
		}
		process -> next_new = actual;
		process -> previous_new = previous;
		previous -> next_new = process;
		if (actual){
			actual -> previous_new = process;
		}
		else{
			queue -> tail = process;
		}
	}
	queue -> count++;	
}

void finished_queue_insert(Queue* queue, Process* process){
	if(queue -> count == 0)
	{
		queue -> tail = process;
		queue -> head = process;
	}
	else
	{
		queue -> tail -> next_finished = process;      
		queue -> tail = process;                               
		queue -> tail -> next_finished = NULL;
	}
	queue -> count++;
}

void queues_insert(Queue* queue, Process* process){
	if(queue -> count == 0)
	{
		queue -> tail = process;
		queue -> head = process;
	}
	else
	{
		queue -> tail -> next_process = process;            //el siguiente del ultimo actual es new
		queue -> tail = process;                                //se agrega new al final
		queue -> tail -> next_process = NULL;
	}
	queue -> count++;
}

int reading_file(char* file, Queue* new_queue, int total_processes){
	int PID = 0;
    char *p;
	FILE* fr = fopen(file, "r");
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
  	if (fr == NULL){
    	return -1;
	}
	while ((read = getline(&line, &len, fr)) != -1) {
		int i;
		Process* process = process_init(PID);
		total_processes += 1;
		PID ++;
		p = strtok(line, " ");
	    if(p)
	    {
	    	strcpy(process->name, p);
	    }
	    p = strtok(NULL, " ");
	    if(p){
	    	process->init_time = atoi(p);
	    }
	    p = strtok(NULL, " ");
	    int limit = atoi(p);
	   	if(p){
	    	for (i = 0; i < limit; i++){
	    		p = strtok(NULL, " ");
	    		if (p){
	    			Burst* burst = burst_init(atoi(p));
	    			burst_insert(process->bursts, burst);
	    		}
	    	}
	    }
	    new_queue_insert(new_queue, process);
	}
	fclose(fr);
	return total_processes;
}

Queue* unir_queues(Queue* menor, Queue* mayor){
	if (mayor -> head == NULL){
		printf("MNEOR %s;\n", menor -> head -> name);
		Queue* aux_menor = menor;
		mayor = aux_menor;
	}
	else{
		Process *head = menor->head;
		Process *tail = menor->tail;
		mayor -> tail -> next_process = head;
		mayor -> tail = tail;		
	}
	// menor -> tail = NULL;
	// menor -> head = NULL;
	mayor -> count += menor -> count;
	menor -> count = 0;
	return mayor;
	// mayor -> tail = menor -> head;
}

int revision_finished(Queue* finished, Process* process){
	Process*actual = finished -> head;
	while (actual != NULL){
		if (actual -> PID == process -> PID){
			return 1;
		}
	}
	return 0;
}