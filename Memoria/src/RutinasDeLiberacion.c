/*
 * RutinasDeLiberacion.c
 *
 *  Created on: 15 jun. 2019
 *      Author: fdalmaup
 */

#include "RutinasDeLiberacion.h"

void liberarMCB(void* MCBAdestruir) {
	if ((MCB_t *) MCBAdestruir != NULL)
		free(MCBAdestruir);
}

void liberarMemorias(void* MemoryAdestruir) {
	if ((t_list *) MemoryAdestruir != NULL) {
		free(((knownMemory_t*) MemoryAdestruir)->ip);
		free(((knownMemory_t*) MemoryAdestruir)->ip_port);
		free(MemoryAdestruir);
	}


}


void liberarRegistroTablaPags(void* registroAdestruir) {
	MCB_t* MCBLibre;

	bool compararConMCB(void* MCBAComparar){
		if((((registroTablaPag_t *) registroAdestruir)->nroMarco) == ((MCB_t *)MCBAComparar)->nroMarco){
			return true;
		}else{
			return false;
		}
	}

	t_list* listaConMCBBuscado = list_filter(memoriaPrincipal.listaAdminMarcos,compararConMCB);

	if (list_is_empty(listaConMCBBuscado)) {
			list_destroy(listaConMCBBuscado);
			//VER SI HAY ALGO PARA RETORNAR
	}
	MCBLibre= (MCB_t*) list_remove(listaConMCBBuscado, 0);

	list_destroy(listaConMCBBuscado);

	pthread_mutex_lock(&mutexColaMarcos);
	queue_push(memoriaPrincipal.marcosLibres, (MCB_t*)MCBLibre);
	pthread_mutex_unlock(&mutexColaMarcos);

	if ((registroTablaPag_t *) registroAdestruir != NULL)
		free(registroAdestruir);
}

void liberarSegmento(void* segmentoAdestruir) {
	if (((segmento_t *) segmentoAdestruir)->pathTabla != NULL)
		free(((segmento_t *) segmentoAdestruir)->pathTabla);

	list_destroy_and_destroy_elements(((segmento_t *) segmentoAdestruir)->tablaPaginas->registrosPag, liberarRegistroTablaPags);

	if (((segmento_t *) segmentoAdestruir)->tablaPaginas != NULL)
		free(((segmento_t *) segmentoAdestruir)->tablaPaginas);

	if ((segmento_t *) segmentoAdestruir != NULL)
		free(segmentoAdestruir);
}

void liberarRecursos(void) {
	log_info(logger_visible,"Finalizando proceso Memoria...");
	printf(GRN"\n  #####                               #######         \n #     #    ##    #    #  ######      #     #  ###### \n #         #  #   ##  ##  #           #     #  #      \n #  ####  #    #  # ## #  #####       #     #  #####  \n #     #  ######  #    #  #           #     #  #      \n #     #  #    #  #    #  #           #     #  #      \n  #####   #    #  #    #  ######      #######  #      \n"STD);
	printf("\n");
	printf(GRN"#######                                                 \n   #     #    #  #####   ######    ##    #####    ####  \n   #     #    #  #    #  #        #  #   #    #  #      \n   #     ######  #    #  #####   #    #  #    #   ####  \n   #     #    #  #####   #       ######  #    #       # \n   #     #    #  #   #   #       #    #  #    #  #    # \n   #     #    #  #    #  ######  #    #  #####    ####   \n\n"STD);
	if (memoriaPrincipal.memoria != NULL)
		free(memoriaPrincipal.memoria);

	pthread_mutex_lock(&mutexTablaSegmentos);

	if(list_is_empty(tablaSegmentos.listaSegmentos)){
		list_destroy(tablaSegmentos.listaSegmentos);
	}else{
		list_destroy_and_destroy_elements(tablaSegmentos.listaSegmentos, liberarSegmento);
	}

	pthread_mutex_unlock(&mutexTablaSegmentos);

	liberarIPs(IPs);
	liberarIPs(IPsPorts);
	queue_clean(memoriaPrincipal.marcosLibres);
	queue_destroy(memoriaPrincipal.marcosLibres);

	list_destroy_and_destroy_elements(memoriaPrincipal.listaAdminMarcos,liberarMCB);
	list_destroy_and_destroy_elements(listaMemoriasConocidas,liberarMemorias);


	if (pathLFS != NULL)
		free(pathLFS);

	pthread_mutex_destroy(&mutexMemoria);
	pthread_mutex_destroy(&mutexTablaSegmentos);
	pthread_mutex_destroy(&mutexColaMarcos);

	config_destroy(configFile);
	log_destroy(logger_invisible);
	log_destroy(logger_visible);

	close(memorySocket);
}

void removerSegmentoDeTabla(segmento_t* segmentoSeleccionado) {
	bool segmentoCoincidePath(void* comparado) {
		//printf(RED"pathSegmentoBuscado: %s\npathComparado: %s"STD"\n",pathSegmentoBuscado,obtenerPath((segmento_t*)comparado));
		if (strcmp(obtenerPath(segmentoSeleccionado),
				obtenerPath((segmento_t*) comparado))) {
			return false;
		}
		return true;
	}

	//TODO: CONDICION DE CARRERA?

	list_remove_by_condition(tablaSegmentos.listaSegmentos,
			segmentoCoincidePath);

}
