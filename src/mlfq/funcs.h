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
  char *name[256];
  int state;     //0 NEW; 1 READY; 2 RUNNING; 3 FINISHED
  int init_time;
  Bursts* bursts;
  // Node* pi;
  // Node* next_node;
};
//
//
// /****************************************************************************/
// /*                               Funciones                                  */
// /****************************************************************************/
//
Process* process_init();
Burst* burst_init();
Bursts* bursts_init();

void burst_insert(Bursts* bursts, Burst* burst);

// void buscar_vecinos(Image* map, Node* node);
// void dfs(Node *puntero, Node* v, int nuclei_count);
// int dfsVisit(Node* u, int tiempo, Node* V, int nuclei_count);
//
// void crear_grafo(Node *puntero, Node* v, int nuclei_count);
// void escribir_grafo(Node* V, int nuclei_count, char* graph_filepath);
//
// double calcular_distancia(int col_i, int row_i, int col_f, int row_f);
// void FW(Node* V, int nuclei_count, char* query_filepath, char* route_filepath);
// void node_insert(Ruta* ruta, Node* node);