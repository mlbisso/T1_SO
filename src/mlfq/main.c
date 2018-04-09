#include <stdio.h>
#include <stdlib.h>
#include "funcs.h"
#include <unistd.h>
#include <string.h>


int main(int argc, char *argv[])
{
	if (((argc != 5 || strcmp(argv[1],"v1") != 0) && (argc != 6 || (strcmp(argv[1], "v2") != 0 && strcmp(argv[1],"v3") != 0))) || atoi(argv[4]) <= 0)
	{
		printf("Uso: %s <version> <file> <quantum> <queues> [<S>] \nDonde\n", argv[0]);
		printf("\t <version> corresponde a v1, v2 o v3 (dependiendo de la versión que se desee ejecutar).\n");
		printf("\t <file> corresponde al nombre de archivo que se debe leer como input. \n");
		printf("\t <quantum> es un parámetro que corresponde al valor de q. \n");
		printf("\t <queues> corresponde al número de colas que usará el algoritmo (Q). Debe ser mayor a 0\n");
		printf("\t [<S>] es un parámetro que corresponde al periodo S. Se necesita solo en v2 y v3\n");
		return 1;
	}

	int quantum = atoi(argv[3]);
	int total_processes = 0;

	int S = 1000000;
	int S_aux = 0;

	if (strcmp(argv[1], "v1") != 0){
		S = atoi(argv[5]);
	}

	//creacion cola de todos los procesos, ordenados por tiempo de llegada
	Queue* new_queue = queue_init(0, -1);

	//creacion de cola de procesos terminados
	Queue* finished_queue = queue_init(3, -1);

	//creacion del sistema de todas las colas de MLFQ
	int Q = atoi(argv[4]);  //numero de colas
	Queue* queues[Q];
	for (int i = 0; i < Q; i++){
		Queue* ready_queue = queue_init(1, i);
		queues[i] = ready_queue;
	}

	//LECTURA ARCHIVO Y CREACION PROCESOS
	total_processes = reading_file(argv[2], new_queue, total_processes);

	//INICIO SIMULACION
	int T = 0;         //tiempo de simulacion
	while(finished_queue->count != total_processes){
		printf("\n");
		printf("TIEMPO ACTUAL: %d\n", T);
			for (int i = Q - 1; i >= 0; i--){
				if (queues[i] -> head != NULL){       //si encuentro un proceso
					Process* actual = queues[i] -> head;
					while(actual != NULL){
						printf("proceso %s\n", actual->name);
						actual = actual -> next_process;
					}
				}
			}
		if (S == S_aux){
			S_aux = 0;
			for (int i = 0; i <= Q - 1; i++){         //busca en todas las colas de MLFQ para disminuir el tiempo de 1° proceso que encuentre
				if (queues[i]->head == NULL){
					continue;
				}	
				else{
					if (i == Q - 1){
						break;
					}
					else{
						queues[i+1] = unir_queues(queues[i], queues[i+1]);
						queues[i] = queue_init(1,i);
					}
				}
			}
		}
		if (new_queue -> count != 0 && T == new_queue ->head -> init_time){     //si un proceso es creado
			printf("%s creado\n", new_queue -> head -> name);
			printf("%s en estado READY\n", new_queue -> head -> name);
			new_queue -> head -> state = 1;
			queues_insert(queues[Q - 1], new_queue-> head);   //se agrega a la primera cola si es tiempo de llegar
			new_queue -> head = new_queue -> head -> next_new;   //se cambia el head de los procesos por llegar
			new_queue -> count -= 1;
			if (new_queue -> count != 0){
				new_queue -> head -> previous_new = NULL;
			}
		}
		for (int i = Q - 1; i >= 0; i--){         //busca en todas las colas de MLFQ para disminuir el tiempo de 1° proceso que encuentre
			if (queues[i] -> head != NULL){       //si encuentro un proceso
				if (strcmp(argv[1], "v1") != 0){
					if (revision_finished(finished_queue, queues[i]->head) == 1){
						continue;
					}
				}				
				if ((queues[i] -> head -> state == 1)){   //quiere decir que Scheduler elige ese proceso
					printf("%s elegido para ejecutar en CPU\n", queues[i] -> head -> name);
					printf("%s cambia a estado RUNNING\n", queues[i] -> head -> name);
					queues[i] -> head -> state = 2;
					queues[i] -> head -> number_CPU += 1;
					if (queues[i] -> head -> response_time == -1){            //para obtener response_time
						queues[i] -> head->response_time = T - queues[i] -> head->init_time; 
					}
				}
				if (queues[i] -> head -> bursts -> head == NULL){
					printf("%s\n", queues[i] -> head->name);
				}
				if (queues[i] -> head -> bursts -> head -> time == 0){    //si se terminó una ´ráfaga
					remove_burst(queues[i] -> head);    //saco la rafaga

					printf("process %s bursts: %d\n", queues[i] -> head-> name, queues[i] -> head->bursts->count);
					// queues[i] -> head ->processed_time = 0;    //para quantum de cada proceso
					if (queues[i] -> head -> bursts ->count == 0){   //si no quedan rafagas en el proceso
						printf("%s cambia a estado FINISHED\n", queues[i] -> head->name);
						queues[i] -> head -> state = 3;
						printf("%s termina\n", queues[i] -> head->name);
						queues[i] -> head->turnaround_time = T - queues[i] -> head->init_time; 
						finished_queue_insert(finished_queue, queues[i] -> head);     //se termino el proceso
						remove_process(queues[i]);
					}
					else{                              //si quedan rafagas, muevo al final de la cola
						printf("%s cambia a estado READY\n", queues[i] -> head->name);
						queues[i] -> head -> state = 1;
						if (queues[i] -> head -> processed_time == quantum){     //si justo hay problemas con quantum
							queues[i] -> head->number_interruptions += 1;	
							if (i != 0){                                       //bajo de cola
								printf("%s baja a prioridad %d\n", queues[i] -> head->name, i-1);					
								Process* proceso_cambiado =  queues[i] -> head; 
								proceso_cambiado-> processed_time = 0;	
								remove_process(queues[i]);
								insert_process(queues[i - 1], proceso_cambiado);	
							}
							else{                                //RR
								queues[i]->  head-> processed_time = 0;				
								move_process(queues[i]);
					    	}
						}
						else{
							move_process(queues[i]);
						}
					}
					T -= 1;
					S_aux -= 1;
					break;
				}
				if (queues[i] -> head -> processed_time == quantum ){   //quiere decir que hay que bajar de prioridad el proceso
					printf("%s cambia a estado READY\n", queues[i] -> head->name);
					queues[i] -> head -> state = 1;
					queues[i] -> head->number_interruptions += 1;	
					if (i != 0){                                       //bajo de cola
						printf("%s baja a prioridad %d\n", queues[i] -> head->name, i-1);					
						Process* proceso_cambiado =  queues[i] -> head; 
						proceso_cambiado-> processed_time = 0;	
						remove_process(queues[i]);
						insert_process(queues[i - 1], proceso_cambiado);	
					}
					else{                                //RR
						queues[i]->  head-> processed_time = 0;				
						move_process(queues[i]);
					}
					T -= 1;
					S_aux -= 1;
					break;
				}
				queues[i] -> head -> bursts -> head -> time -= 1;    //le bajo el tiempo en 1 
				queues[i] -> head -> processed_time += 1;
				queues[i] -> head -> in_cpu += 1;
				break;
			}
		}
		T += 1;
		S_aux += 1;
		for (int i = Q - 1; i >= 0; i--){   //para obtener los readys
			recorrer_ready(queues[i]);
		}
	}

	//ESCRIBIR INFO EN ARCHIVO
	FILE *f = fopen("result.txt", "w");   //TODO cambiar nombre archivo
	fprintf(f, "Procesos terminados: %d\n", finished_queue->count);
	fprintf(f, "Tiempo total: %d\n\n", T);
	int total = finished_queue->count;	
	for(int i = 0; i < total; i++){
	 	  fprintf(f, "%s:\n", finished_queue->head->name);
	 	  fprintf(f, "Turnos de CPU: %d\n", finished_queue->head->number_CPU);
	 	  fprintf(f, "Bloqueos: %d\n", finished_queue->head->number_interruptions);
	 	  fprintf(f, "Turnaround time: %d\n", finished_queue->head->turnaround_time);
	 	  fprintf(f, "Response time: %d\n", finished_queue->head->response_time);
	 	  fprintf(f, "Waiting time: %d\n\n", finished_queue->head->waiting_time);
	 	  finished_remove_process(finished_queue);
	}
	fclose(f);
	return 0;
}
