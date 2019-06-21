/*
 * hash_sc.c
 *
 *  Created on: 21 jun. 2019
 *      Author: facusalerno
 */

#include "hash_sc.h"

hash getHash(char *imput, unsigned int limite){
	if(limite < 1)
		return 0;
	int acumulador = 1;
	for(int i=0; i<strlen(imput); ++i){
		acumulador += imput[i];
	}
	return (acumulador % (limite - 1))-1;
}
