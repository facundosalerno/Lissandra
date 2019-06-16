/*
 * LFS.h
 *
 *  Created on: 20 may. 2019
 *      Author: juanmaalt
 */

#ifndef LISSANDRA_H_
#define LISSANDRA_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libgen.h>

#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

#include <cliente/cliente.h>
#include <parser/parser_comando.h>
#include <server_multithread/server_multithread.h>
#include <epoch/epoch.h>

#include <readline/readline.h>
#include <pthread.h>
#include <semaphore.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "adhoc/Consola.h"
#include "adhoc/APILissandra.h"


#define RED "\x1b[31m"
#define STD "\x1b[0m"

//ESTRUCTURAS
typedef struct{
	char *ip;
	char *puerto_escucha;
	char *ip_memoria;
	char *puerto_memoria;
	char *punto_montaje;
	char *retardo; //Esta config la tengo que dejar como variable.
	char *tamanio_value;
	//char *tiempo_dump; //Esta config la dejamos como variable.
}Config_final_data;

typedef unsigned long long timestamp_t;
typedef struct{
	timestamp_t timestamp;
	uint16_t key;
	char* value;
}Registro;

//GLOBALES
t_log* logger_visible;
t_log* logger_invisible;
t_config* configFile;
Config_final_data config;
int socketMemoria;

pthread_t idConsola;


t_dictionary* memtable;

//FUNCIONES
int configuracion_inicial();
t_log* iniciar_logger(bool);
t_config* leer_config();
void extraer_data_config();
void ver_config();
void crearDirectorioDeMontaje(char* puntoMontaje);
char* directorioTablas();
int crearDirectorio(char* path);
t_dictionary* inicializarMemtable();
void handshakeMemoria(int socket);
int threadConnection(int serverSocket, void *funcionThread);
int iniciar_consola();
Operacion ejecutarOperacion(char*);

void *connection_handler(void *nSocket);

void agregarDatos(t_dictionary* memtable);

uint16_t obtenerKey(Registro* registro);
timestamp_t obtenerTimestamp(Registro* registro);

#endif /* LISSANDRA_H_ */

