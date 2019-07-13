/*
 * FuncionesAPI.c
 *
 *  Created on: 23 jun. 2019
 *      Author: juanmaalt
 */

#include "FuncionesAPI.h"

/*INICIO FUNCIONES COMPLEMENTARIAS*/
bool existeTabla(char* nombreTabla){
	return dictionary_has_key(memtable, nombreTabla);
}


int getMetadata(char* nombreTabla, t_config* metadataFile){
	if(metadataFile == NULL){
		printf(RED"APILissandra.c: leerMetadata: error en el archivo 'Metadata' de la tabla %s"STD"\n", nombreTabla);
		return EXIT_FAILURE;
	}

	extraerMetadata(metadataFile);

	//mostrarMetadata();//funcion adhoc para testing

	return EXIT_SUCCESS;
}


t_config* leerMetadata(char* nombreTabla){
	char* path = string_from_format("%sTables/%s/Metadata", config.punto_montaje, nombreTabla);

	return config_create(path);
}


void extraerMetadata(t_config* metadataFile) {
	metadata.compaction_time = config_get_int_value(metadataFile, "COMPACTION_TIME");
	metadata.consistency = config_get_string_value(metadataFile, "CONSISTENCY");
	metadata.partitions= config_get_int_value(metadataFile, "PARTITIONS");
}


void mostrarMetadata(){
	log_info(logger_visible, "\nMetadata->compaction_time= %d\n", metadata.compaction_time);
	log_info(logger_visible, "Metadata->consistency= %s\n", metadata.consistency);
	log_info(logger_visible, "Metadata->partitions= %d\n", metadata.partitions);
}


t_list*	getData(char* nombreTabla){
	return dictionary_get(memtable, nombreTabla);
}


int calcularParticionNbr(char* key, int particiones){
	return atoi(key)%particiones;
}


t_list* buscarValueEnLista(t_list* data, char* key){
	//printf("Numero de partición: %d\n\n", particionNbr);

	bool compararConItem(void* item){
		if (atoi(key) != obtenerKey((Registro*) item)) {
			return false;
		}
		return true;
	}

	return list_filter(data, compararConItem);
}
/*
void buscarValueEnFiles(char* nombreTabla, char* key, int particionNbr, t_list* listaDeValues){
	char* path = string_from_format("%sTables/%s", config.punto_montaje, nombreTabla);
	char* listaDeBloques= obtenerListaDeBloques(particionNbr, nombreTabla);
	char** bloques = string_get_string_as_array(listaDeBloques);

	FILE* fBloque;
    int timestamp, fkey;
    char value[atoi(config.tamanio_value)];
	int i=0;

	while(bloques[i]!=NULL){
		char* pathBloque = string_from_format("%sBloques/%s.bin", config.punto_montaje, bloques[i]);
		fBloque = fopen(pathBloque, "r");
		while(fBloque!= EOF){
			fscanf(fBloque, "%d;%d;%[^\n]s", &timestamp, &fkey, value);
			if(fkey==atoi(key)){
				printf("key: %d\n");
			}
		}
	}

}
*/

void leerTemps(char* nombreTabla, char* key, t_list* listaDeValues){
	char* pathTabla = string_from_format("%sTables/%s", config.punto_montaje, nombreTabla);

	DIR *dir;
	struct dirent *entry;
	char* nombreTemp;

	FILE* temp;
    int fkey;
    timestamp_t timestamp;
    char value[atoi(config.tamanio_value)];
    if((dir = opendir(pathTabla)) != NULL){
		while((entry = readdir (dir)) != NULL){
			nombreTemp = string_from_format(entry->d_name);
			if(string_contains(nombreTemp, ".tmp")){
				char* pathTemp = string_from_format("%s/%s", pathTabla, nombreTemp);
				temp = fopen(pathTemp, "r");
				while(fscanf(temp, "%llu;%d;%[^\n]s", &timestamp, &fkey, value)!= EOF){
					if(fkey==atoi(key)){
						Registro* reg = malloc(sizeof(Registro));
						reg->key = fkey;
						reg->value = string_from_format(value);
						reg->timestamp= timestamp;

						list_add(listaDeValues, reg);
					}
				}
			}
		}
	}
}


void recorrerTabla(t_list* lista){
	Registro* reg= NULL;
	int i=0;

	while(!(list_get(lista, i)==NULL)){
		reg = list_get(lista, i);

		printf("Registro->Timestamp= %llu\n", reg->timestamp);
		printf("Registro->Key= %d\n", reg->key);
		printf("Registro->Value= %s\n", reg->value);

		i++;
	}
}


void getValueMasReciente(t_list* lista, Operacion* resultadoSelect){
	if(list_size(lista)>0){
		if(list_size(lista)>1){
			bool compararFechas(void* item1, void* item2){
				if (obtenerTimestamp((Registro*) item1) < obtenerTimestamp((Registro*) item2)){
				return false;
				}
			return true;
			}
			list_sort(lista, compararFechas);
		}

		Registro* reg = list_get(lista, 0);

		resultadoSelect->TipoDeMensaje = REGISTRO;
		resultadoSelect->Argumentos.REGISTRO.timestamp=reg->timestamp;
		resultadoSelect->Argumentos.REGISTRO.key=reg->key;
		resultadoSelect->Argumentos.REGISTRO.value=string_from_format("%s",reg->value);

	}else{
		resultadoSelect->TipoDeMensaje = ERROR;
		resultadoSelect->Argumentos.ERROR.mensajeError = string_from_format("No existen registros relacionados con la key solicitada");
	}
}


timestamp_t checkTimestamp(char* timestamp){
	if(timestamp==NULL){
		return getCurrentTime();
	}
	else{
		//printf("time= %llu\n",atoll(timestamp));
		return atoll(timestamp);
	}
}


void crearTablaEnMemtable(char* nombreTabla){
	t_list* lista = list_create();
	char* tabla=nombreTabla;

	dictionary_put(memtable, tabla, lista);
}


void setPathTabla(char* path, char* nombreTabla){
	strcpy(path,config.punto_montaje);
	strcat(path, "Tables/");
	strcat(path, nombreTabla);
}

void crearDirectorioTabla(char* path){
	crearDirectorio(path);
	strcat(path,"/");
}


void crearArchivo(char* path, char* nombre){
	char* pathArchivo = malloc(110 * sizeof(char));
	strcpy(pathArchivo,path);
	strcat(pathArchivo, nombre);

	//printf("archivo creado en: %s\n", pathArchivo);

	FILE* file=NULL;
	file = fopen(pathArchivo,"w");

	fclose(file);
	free(pathArchivo);
}

void escribirArchivoMetadata(char* path, Comando comando){
	FILE* fmetadata;

	char* pathArchivo = malloc(110 * sizeof(char));

	strcpy(pathArchivo,path);
	strcat(pathArchivo, "Metadata");

	fmetadata = fopen(pathArchivo,"a");

	fprintf (fmetadata, "COMPACTION_TIME=%d\n",atoi(comando.argumentos.CREATE.compactacionTime));
	fprintf (fmetadata, "CONSISTENCY=%s\n",comando.argumentos.CREATE.tipoConsistencia);
	fprintf (fmetadata, "PARTITIONS=%d\n",atoi(comando.argumentos.CREATE.numeroParticiones));

	fclose(fmetadata);
	free(pathArchivo);
}

void crearArchivosBinarios(char* path, int particiones){
	char* pathArchivo = string_from_format(path);
	char* filename=string_new();
	FILE* binario;

	for(int i=0;i<particiones;i++){
		filename=string_from_format("%d.bin", i);
		crearArchivo(path, filename);
		string_append(&pathArchivo, filename);

		int bloque = getBloqueLibre();
		binario = txt_open_for_append(pathArchivo);
		txt_write_in_file(binario, string_from_format("SIZE=0\nBLOCKS=[%d]\n",bloque));
	}
	fclose(binario);
}

void insertInFile(char* path, int particionNbr, char* key, char* value){
	FILE* fParticion;

	char* filename=string_from_format("%d.bin", particionNbr);
	char* pathArchivo = string_from_format("%s/%s", path, filename);

	fParticion = fopen(pathArchivo,"a");

	char* keyValue = string_from_format("%s;%s", key, value);

	fprintf (fParticion, "%s",keyValue);

	free(keyValue);
	fclose(fParticion);
}

void getStringDescribe(char* path, char* pathMetadata, char* string, char* nombreTabla, Operacion *resultadoDescribe){
	DIR *dir;
	struct dirent *entry;
	char* nombreCarpeta;
	t_config* metadata;

	if(nombreTabla==NULL){
		if((dir = opendir(path)) != NULL){
			while((entry = readdir (dir)) != NULL){
				nombreCarpeta = string_from_format(entry->d_name);
				if(!strcmp(nombreCarpeta, ".") || !strcmp(nombreCarpeta, "..")){
				}else{
					//printf("path: %s\n", pathMetadata);
					//printf("nombreTabla: %s\n", nombreCarpeta);

					metadata = config_create(string_from_format("%s%s/Metadata", pathMetadata, nombreCarpeta));
					char* consistencia = string_from_format(config_get_string_value(metadata, "CONSISTENCY"));
					int compactionTime=config_get_int_value(metadata, "COMPACTION_TIME");
					int particiones =config_get_int_value(metadata, "PARTITIONS");

					concatenar_tabla(&string, nombreCarpeta, consistencia, particiones, compactionTime);
					//printf("string: %s\n", string);

					strcpy(pathMetadata,path);
					config_destroy(metadata);
				}
		  }
			closedir (dir);
			if(string!=NULL){
				resultadoDescribe->Argumentos.DESCRIBE_REQUEST.resultado_comprimido = string_from_format(string);
				resultadoDescribe->TipoDeMensaje= DESCRIBE_REQUEST;
			}else{
				resultadoDescribe->Argumentos.DESCRIBE_REQUEST.resultado_comprimido = NULL;
			}
		}else{
			resultadoDescribe->Argumentos.DESCRIBE_REQUEST.resultado_comprimido = NULL;
		}
	}else{
		if((dir = opendir (path)) != NULL){
			//printf("path: %s\n", pathMetadata);
			//printf("nombreTabla: %s\n", nombreTabla);

			metadata = config_create(string_from_format("%s%s/Metadata", pathMetadata, nombreTabla));
			char* consistencia = string_from_format(config_get_string_value(metadata, "CONSISTENCY"));
			int compactionTime=config_get_int_value(metadata, "COMPACTION_TIME");
			int particiones =config_get_int_value(metadata, "PARTITIONS");

			concatenar_tabla(&string, nombreTabla, consistencia, particiones, compactionTime);
			//printf("string: %s\n", string);
			resultadoDescribe->TipoDeMensaje= DESCRIBE_REQUEST;
			resultadoDescribe->Argumentos.DESCRIBE_REQUEST.resultado_comprimido = string_from_format(string);
			config_destroy(metadata);
		}
		else{
			resultadoDescribe->Argumentos.DESCRIBE_REQUEST.resultado_comprimido = NULL;
		}
	}

	//printf("string final: %s\n", string);
}

int removerDirectorio(char *path){
   DIR *dir = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;

   if (dir)   {
      struct dirent *p;
      r = 0;

      while (!r && (p=readdir(dir))){
          int r2 = -1;
          char *buf;
          size_t len;

          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")){
             continue;
          }

          len = path_len + strlen(p->d_name) + 2;
          buf = malloc(len);

          if (buf){
             struct stat statbuf;

             snprintf(buf, len, "%s/%s", path, p->d_name);

             if (!stat(buf, &statbuf)){
                if (S_ISDIR(statbuf.st_mode)){
                   r2 = removerDirectorio(buf);
                }else{
                	r2 = unlink(buf);
                }
             }
             free(buf);
          }
          r = r2;
      }
      closedir(dir);
   }

   if (!r){
      r = rmdir(path);
   }

   return r;
}

void limpiarBloquesEnBitarray(char* nombreTabla){
	char* pathTablas = string_from_format("%sTables/%s/", config.punto_montaje, nombreTabla);

	DIR *dir;
	struct dirent *entry;
	char* nombreArchivo;

	if((dir = opendir(pathTablas)) != NULL){
		while((entry = readdir (dir)) != NULL){
			nombreArchivo = string_from_format(entry->d_name);
			if(string_contains(nombreArchivo, ".bin")){
				char* particionNbr =string_substring_until(nombreArchivo, (strlen(nombreArchivo)-4));
				char* listaDeBloques= obtenerListaDeBloques(atoi(particionNbr), nombreTabla);
				char** bloques = string_get_string_as_array(listaDeBloques);

				int i=0;

				while(bloques[i]!=NULL){
					int pos = atoi(bloques[i]);
					bitarray_clean_bit(bitarray, (pos-1));
					i++;
				}
			}
		}
	}

}

int iniciarCompactacion(char* nombreTabla){
	pthread_t idCompactacion;
	if (pthread_create(&idCompactacion, NULL, compactar, nombreTabla)) {
		log_error(logger_error, "FuncionesAPI.c: iniciarCompactacion(): Falló al iniciar el hilo de compactación");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}


/*FIN FUNCIONES COMPLEMENTARIAS*/
