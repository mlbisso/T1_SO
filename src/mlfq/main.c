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
		PID ++;
		p = strtok(line, " ");
	    if(p)
	    {
	    	*process->name = p;
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
	}
	fclose(fr);
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
