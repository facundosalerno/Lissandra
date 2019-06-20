/*
 * Consola.c
 *
 *  Created on: 20 may. 2019
 *      Author: juanmaalt
 */

#include "Consola.h"

void mostrarRetorno(Operacion retorno) {
	switch (retorno.TipoDeMensaje) {
	case REGISTRO:
		printf("Timestamp: %llu\nKey:%d\nValue: %s\n",
				retorno.Argumentos.REGISTRO.timestamp,
				retorno.Argumentos.REGISTRO.key,
				retorno.Argumentos.REGISTRO.value);
		return;
	case TEXTO_PLANO:
		printf("Resultado: %s\n",retorno.Argumentos.TEXTO_PLANO.texto);
		return;
	case ERROR:
		printf("ERROR: %s \n",retorno.Argumentos.ERROR.mensajeError);
		return;
	case COMANDO:
		return;
	}
}

void *recibir_comandos(void *null){
	pthread_detach(pthread_self());

	Operacion retorno;
	for(;;){
		char *userInput = readline("> ");
		retorno = ejecutarOperacion(userInput);
		mostrarRetorno(retorno);
		destruir_operacion(retorno);
		free(userInput);
	}
	return NULL;
}



