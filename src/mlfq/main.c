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
	int PID = 0;
    char *p;
	FILE* fr = fopen(argv[2], "r");
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
  	if (fr == NULL){
    	return 2;
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

	//PARA PROBAR NEW_QUEUE
	// Process* actual = new_queue->head;
	// while (actual){
	// 	printf("%d\n", actual ->init_time);
	// 	actual = actual -> next_new;
	// }
	// printf("hola\n");
	// printf("%d\n", new_queue->head->PID);
	// printf("%d\n", new_queue->tail->PID);

	//INICIO SIMULACION
	int T = 0;         //tiempo de simulacion
	while(finished_queue->count != total_processes){
		printf("\n");
		printf("TIEMPO ACTUAL: %d\n", T);
		if (new_queue -> count != 0 && T == new_queue ->head -> init_time){     //si un proceso es creado
			printf("%s creado\n", new_queue -> head -> name);
			printf("%s en estado READY\n", new_queue -> head -> name);
			queues_insert(queues[Q - 1], new_queue-> head);   //se agrega a la primera cola si es tiempo de llegar
			new_queue -> head = new_queue -> head -> next_new;   //se cambia el head de los procesos por llegar
			// printf("%d\n", new_queue -> head -> PID);
			new_queue -> count -= 1;
			if (new_queue -> count != 0){
				new_queue -> head -> previous_new = NULL;
			}

		}
		for (int i = Q - 1; i >= 0; i--){         //busca en todas las colas de MLFQ para disminuir el tiempo de 1° proceso que encuentre
			if (queues[i] -> head != NULL){       //si encuentro un proceso
				if (queues[i] -> head -> processed_time == 0){   //quiere decir que Scheduler elige ese proceso
					printf("%s elegido para ejecutar en CPU\n", queues[i] -> head -> name);
					printf("%s cambia a estado RUNNING\n", queues[i] -> head -> name);
					queues[i] -> head -> number_CPU += 1;
					if (queues[i] -> head -> response_time == 0){            //para obtener response_time
						queues[i]->head->response_time = T - queues[i]->head->init_time; 
					}
				}
				queues[i] -> head -> bursts -> head -> time -= 1;    //le bajo el tiempo en 1 
				if (queues[i] -> head -> bursts -> head -> time == 0){    //si se terminó una ´ráfaga
					remove_burst(queues[i] -> head);    //saco la rafaga
					queues[i] -> head ->processed_time = 0;    //para quantum de cada proceso
					if (queues[i] -> head -> bursts ->count == 0){   //si no quedan rafagas en el proceso
						printf("%s cambia a estado FINISHED\n", queues[i]->head->name);
						printf("%s termina\n", queues[i]->head->name);
						queues[i]->head->turnaround_time = T - queues[i]->head->init_time; 
						finished_queue_insert(finished_queue, queues[i] -> head);     //se termino el proceso
						remove_process(queues[i]);
					}
					else{                              //si quedan rafagas, muevo al final de la cola
						printf("%s cambia a estado READY\n", queues[i]->head->name);
						move_process(queues[i]);
					}
					break;
				}
				queues[i] -> head -> processed_time += 1;
				if (queues[i] -> head -> processed_time == quantum ){   //quiere decir que hay que bajar de prioridad el proceso
					printf("%s cambia a estado READY\n", queues[i]->head->name);
					queues[i]->head->number_interruptions += 1;					
					if (i != 0){                                       //bajo de cola
						printf("%s baja a prioridad %d\n", queues[i]->head->name, i-1);					
						Process* proceso_cambiado =  queues[i] -> head; 
						proceso_cambiado-> processed_time = 0;
						insert_process(queues[i - 1], proceso_cambiado);
						remove_process(queues[i]);						
					}
					else{                                //RR
						move_process(queues[i]);
					}
				}
				break;
			}
		}
		T += 1;
	}


	//
	// /* Las rutas a los distintos archivos */
	// char* map_filepath = argv[1];
	// char* query_filepath = argv[2];
	// char* graph_filepath = argv[3];
	// char* route_filepath = argv[4];
	//
	// /** Lee la imagen */
	// Image* map = img_png_read_from_file(map_filepath);
	// // watcher_open(map_filepath);
	//
	// /** Cantidad de núcleos */
	// int nuclei_count = 0;
	// for(int row = 0; row < map -> height; row++){
	// 	for(int col = 0; col < map -> width; col++){
	// 		/* Si es 0 es negro */
	// 		if(map -> pixels[row][col] == 0)
	// 		{
	// 			/* Encontramos un núcleo! */
	// 			// printf("El pixel (%d,%d) corresponde a un núcleo\n", col, row);
	// 			nuclei_count++;
	// 		}
	// 	}
	// }
	//
	// Node V[nuclei_count + 1];
	// //inicializacion
	// for(int i = 0; i < nuclei_count + 1; i++){
	// 	V[i] = *node_init(0, 0, 0, nuclei_count);
	// }
	//
	// for(int row = 0; row < map -> height; row++){
	// 	for(int col = 0; col < map -> width; col++){
	// 		/* Si es 0 es negro */
	// 		if(map -> pixels[row][col] == 0)
	// 		{
	// 			/* Encontramos un núcleo! */
	// 			// printf("El pixel (%d,%d) corresponde a un núcleo\n", col, row);
	// 			int id = 0;
	// 			if (row != 0){
	// 				id = map -> pixels[row - 1][col];
	// 			}
	// 			else{
	// 				id = map -> pixels[row + 1][col];
	// 			}
	// 			Node* node = node_init(id, row, col, nuclei_count);
	// 			buscar_vecinos(map, node);
	// 			V[id] = *node;
	// 		}
	// 	}
	// }
	//
	// // Node* puntero = V;
	// //dfs(puntero, V, nuclei_count);
	// // crear_grafo(puntero, V, nuclei_count);
	//
	// escribir_grafo(V, nuclei_count, graph_filepath);
	//
	// FW(V, nuclei_count, query_filepath, route_filepath);
	// // printf("En total son %d células\n", nuclei_count);
	//
	// /* Libera la imagen */
	// img_png_destroy(map);
	// // usleep(150000);
	// // watcher_close();
	return 0;
}
