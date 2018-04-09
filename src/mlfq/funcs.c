// Importamos la librería estándar de C
#include <stdlib.h>
// Importamos el módulo estándar de I/O
#include <stdio.h>
// Importamos las definiciones de la lista
#include "funcs.h"
#include <math.h>
#include <unistd.h>

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
	process->turnaround_time = 0;
    process->response_time = 0;
    process->waiting_time = 0;
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

// Queues* queues_init(){
// 	Queues* queues = malloc(sizeof(Queues));
// 	queues -> count = 0;
// 	queues -> head = NULL;
// 	queues -> tail = NULL;
// 	return queues;
// }

//

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
	process -> next_process = NULL;
	queue -> tail -> next_process = process;
	queue -> tail = process;
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
		// for (int i = 0; i < queue->count; i++){
		// 	if (actual->time > new->time){
		// 		actual->previous_new->next_new = new; 
		// 		new->next_new = actual;
		// 		new->previous_new = actual -> previous_new
		// 	}
		// }
		// //ingresar process por orden de tiempo
		// bursts -> tail -> next_burst = new;            //el siguiente del ultimo actual es new
		// bursts -> tail = new;                                //se agrega new al final
		// bursts -> tail -> next_burst = NULL;
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

//
// void buscar_vecinos(Image* map, Node* node){
// 	int candidato;
// 	for(int row = 0; row < map -> height; row++){
// 		for(int col = 0; col < map -> width; col++){
// 			if(map -> pixels[row][col] == node->id){
// 				//revisar alrededores del pixel
// 				 	if (row != 0){
// 						candidato = map -> pixels[row - 1][col];
// 						if (node->alpha[candidato] == 0 && candidato != 0 && candidato != node ->id){
// 							node->num_vecinos += 1;
// 							node->alpha[candidato] = candidato;
// 						}
// 					}
// 					if (row != map -> height - 1){
// 						candidato = map -> pixels[row + 1][col];
// 						if (node->alpha[candidato] == 0 && candidato != 0 && candidato != node ->id){
// 							node->num_vecinos += 1;
// 							node->alpha[candidato] = candidato;
// 						}
// 					}
// 					if (col != 0){
// 						candidato = map -> pixels[row][col - 1];
// 						if (node->alpha[candidato] == 0 && candidato != 0 && candidato != node ->id){
// 							node->num_vecinos += 1;
// 							node->alpha[candidato] = candidato;
// 						}
// 					}
// 					if (col != map -> width - 1){
// 						candidato = map -> pixels[row][col + 1];
// 						if (node->alpha[candidato] == 0 && candidato != 0 && candidato != node ->id){
// 							node->num_vecinos += 1;
// 							node->alpha[candidato] = candidato;
// 						}
// 					}
// 			}
// 		}
// 	}
// }
//
// void dfs(Node *puntero, Node* V, int nuclei_count){
// 	int tiempo = 0;
// 	for(int i = 0; i < nuclei_count + 1; i++){
// 		if (V[i].id != 0 && V[i].color == 0){
// 			tiempo = dfsVisit(&V[i], tiempo, V, nuclei_count);
// 		}
// 		puntero ++;
// 	}
// }
//
// int dfsVisit(Node* u, int tiempo, Node* V, int nuclei_count){
// 	u->color = 1;
// 	tiempo += 1;
// 	u->d  = tiempo;
// 	for(int i = 0; i < nuclei_count+ 1; i++){
// 		if (u->alpha[i] != 0 && V[u->alpha[i]].color == 0){
// 			// watcher_draw_node(u ->core_col, u->core_row, V[u->alpha[i]].core_col, V[u->alpha[i]].core_row);
// 			// usleep(5000000);
// 			V[u->alpha[i]].pi = u;
// 			V[u->id] = *u;
// 			tiempo = dfsVisit(&V[u->alpha[i]], tiempo, V, nuclei_count);
// 		}
// 	}
// 	u->color = 2;
// 	tiempo += 1;
// 	u-> f = tiempo;
// 	V[u->id] = *u;
// 	return tiempo;
// }
//
// void crear_grafo(Node *puntero, Node* V, int nuclei_count){
// 	for(int i = 0; i < nuclei_count + 1; i++){
// 		if (V[i].id != 0){
// 			for (int j = 0; j < nuclei_count + 1; j++){
// 				if (V[i].alpha[j] != 0){
// 					// watcher_draw_node(V[i].core_col, V[i].core_row, V[V[i].alpha[j]].core_col, V[V[i].alpha[j]].core_row);
// 					// usleep(5000000);
// 				}
// 			}
// 		}
// 		puntero ++;
// 	}
// }
//
// void escribir_grafo(Node* V, int nuclei_count, char* file){
// 	FILE *f = fopen(file, "w");
// 	double dist = 0;
// 	for(int i = 1; i < nuclei_count + 1; i++){
// 		  fprintf(f, "%d %d ", i, V[i].num_vecinos);
// 			for (int j = 0; j < nuclei_count + 1; j++){
// 				if (V[i].alpha[j] != 0){
// 					dist = calcular_distancia(V[i].core_col, V[i].core_row, V[V[i].alpha[j]].core_col, V[V[i].alpha[j]].core_row);
// 					fprintf(f, "%d %f ", V[i].alpha[j], dist);
// 					V[i].dist_vecinos[V[i].alpha[j]] = dist;
// 				}
// 			}
// 			fprintf(f, "\n");
// 	}
// 	fclose(f);
// }
//
// double calcular_distancia(int col_i, int row_i, int col_f, int row_f){
// 	double dist = 0;
// 	dist = sqrt(pow(col_i - col_f, 2) + pow(row_i - row_f, 2));
// 	return dist;
// }
//
// void FW(Node* V, int nuclei_count, char* query_filepath, char* route_filepath){
// 	double **dist = (double **) malloc((nuclei_count + 1) * sizeof(double *));
// 	int **next = (int **) malloc((nuclei_count + 1) * sizeof(int *));
// 	for (int i = 0; i < nuclei_count + 1; i++){
// 		dist[i] = (double *) malloc((nuclei_count + 1) * sizeof(double));
// 		next[i] = (int *) malloc((nuclei_count + 1) * sizeof(int));
// 	}
// 	for (int u = 0; u < nuclei_count + 1; u++){
// 		for (int v = 0; v < nuclei_count + 1; v++){
// 			if (u == v && u != 0 && v != 0){
// 				dist[u][v] = 0;
// 				next[u][v] = -1;
// 			}
// 			else if (V[u].dist_vecinos[v] != 0){
// 				dist[u][v] = V[u].dist_vecinos[v];
// 				next[u][v] = v;
// 			}
// 			else{
// 				dist[u][v] = INFINITY;
// 				next[u][v] = -1;
// 			}
// 		}
// 	}
//
// 	for (int k = 1; k < nuclei_count + 1; k++){
// 		for (int i = 1; i < nuclei_count + 1; i++){
// 			for (int j = 1; j < nuclei_count + 1; j++){
// 					if (dist[i][j] > dist[i][k] + dist[k][j]){
// 						dist[i][j] = dist[i][k] + dist[k][j];
// 						next[i][j] = next[i][k];
// 					}
// 			}
// 		}
// 	}
//
//  	FILE* query_file = fopen(query_filepath, "r");
//  	FILE* answer = fopen(route_filepath, "w");
//
// 	/* Cantidad de consultas del archivo */
// // 	for(int i = 1; i < nuclei_count + 1; i++){
// // 			fprintf(f, "%d %d ", i, V[i].num_vecinos);
// // 			for (int j = 0; j < nuclei_count + 1; j++){
// // 				if (V[i].alpha[j] != 0){
// // 					dist = calcular_distancia(V[i].core_col, V[i].core_row, V[V[i].alpha[j]].core_col, V[V[i].alpha[j]].core_row);
// // 					fprintf(f, "%d %f ", V[i].alpha[j], dist);
// // 					V[i].dist_vecinos[V[i].alpha[j]] = dist;
// // 				}
// // 			}
// // 			fprintf(f, "\n");
// // 	}
// // 	fclose(f);
// // }
// 	int queries_count;
// 	fscanf(query_file, "%d", &queries_count);
// 	/* Leemos todas las consultas */
// 	//TODO malo
// 	for (int i = 0; i < queries_count; i++)
// 	{
// 		int start;
// 		int end;
// 		Ruta* ruta = ruta_init();
// 		fscanf(query_file, "%d %d", &start, &end);
// 		if (next[start][end] == -1){
// 			//se entrega vacion no ocurre
// 		}
// 		node_insert(ruta, &V[start]);
// 		while (start != end){
// 			start = next[start][end];
// 			node_insert(ruta, &V[start]);
// 		}
// 		fprintf(answer, "%d ", ruta ->count - 2);
// 		Node* node = ruta -> head;
// 		while(node->id != ruta -> tail->id){
// 			fprintf(answer, "%d ", node -> id);
// 			node =  node-> next_node;
// 		}
// 		fprintf(answer, "%d", node -> id);
// 		fprintf(answer, "\n");
// 	}
//   //
// 	// /* Cerrar el archivo */
// 	fclose(query_file);
// 	fclose(answer);
// }
