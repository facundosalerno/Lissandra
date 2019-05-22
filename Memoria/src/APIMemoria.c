/*
 * APIMemoria.c
 *
 *  Created on: 15 may. 2019
 *      Author: fdalmaup
 */

#include "APIMemoria.h"

/*
 * La operación Select permite la obtención del valor de una key dentro de una tabla. Para esto, se utiliza la siguiente nomenclatura:
 SELECT [NOMBRE_TABLA] [KEY]
 Ej:
 SELECT TABLA1 3

 Esta operación incluye los siguientes pasos:
 1. Verifica si existe el segmento de la tabla solicitada y busca en las páginas del mismo si contiene key solicitada.
 Si la contiene, devuelve su valor y finaliza el proceso.

 2. Si no la contiene, envía la solicitud a FileSystem para obtener el valor solicitado y almacenarlo.

 3. Una vez obtenido se debe solicitar una nueva página libre para almacenar el mismo.

 En caso de no disponer de una página libre, se debe ejecutar el algoritmo de reemplazo y,
 en caso de no poder efectuarlo por estar la memoria full, ejecutar el Journal de la memoria.
 *
 * */


void selectAPI(char* input, Comando comando) {
	segmento_t *segmentoSeleccionado = NULL;

	uint16_t keyBuscada = atoi(comando.argumentos.SELECT.key); //TODO: se verifica que la key sea numerica?

	pagina_t *paginaBuscada=NULL;

	char* value;

	if (verificarExistenciaSegmento(comando.argumentos.SELECT.nombreTabla, &segmentoSeleccionado)) {
		if (contieneKey(segmentoSeleccionado, keyBuscada, &paginaBuscada)) {
			value=obtenerValue(paginaBuscada);
			printf("El value es: %s\n",value);
			free(value);
			return; //TODO: se debe devolver el value
		}
		printf(RED"APIMemoria.c: select: no encontro la key. Enviar a LFS la request"STD"\n");
		/*else{
		 //TODO: Enviar a LFS la request
		  * send_msg
		  * recv
		  *
		 //Recibo el valor (el marcoRecibido/registro entero ya parseado al ser recibido como un char*)
		 //
		 solicitarPagina(segmentoSeleccionado,marcoRecibido);
		 }*/

	}/*else{
	 //TODO: Enviar a LFS la request
	 //crearSegmento(segmentoSeleccionado); -> me lo agrega a la tabla de segmentos (inicializar el segmento)
	 //solicitaPagina(segmentoSeleccionado,marcoRecibido)
	 }*/

	printf(RED"APIMemoria.c: select: no encontro el path. Enviar a LFS la request"STD"\n");

	printf("La linea recibida es: %s\n", input);
	printf("La key es: %d\n", keyBuscada);
	printf("NO se encontro el value para la key\n");


}
/*
 * La operación Insert permite la creación y/o actualización de una key dentro de una tabla. Para esto, se utiliza la siguiente nomenclatura:
 INSERT [NOMBRE_TABLA] [KEY] “[VALUE]”
 Ej:
 INSERT TABLA1 3 “Mi nombre es Lissandra”

 Esta operación incluye los siguientes pasos:
 1. Verifica si existe el segmento de la tabla en la memoria principal.
 De existir, busca en sus páginas si contiene la key solicitada y de contenerla actualiza el valor insertando el Timestamp actual.
 En caso que no contenga la Key, se solicita una nueva página para almacenar la misma.
 Se deberá tener en cuenta que si no se disponen de páginas libres
 aplicar el algoritmo de reemplazo (LRU) y en caso de que la memoria se encuentre full iniciar el proceso Journal.

 2. En caso que no se encuentre el segmento en memoria principal, se creará y se agregará la nueva Key con el Timestamp actual,
 junto con el nombre de la tabla en el segmento. Para esto se debe generar el nuevo segmento y solicitar una nueva página
 (aplicando para este último la misma lógica que el punto anterior).

 Cabe destacar que la memoria no verifica la existencia de las tablas al momento de insertar nuevos valores. De esta forma,
 no tiene la necesidad de guardar la metadata del sistema en alguna estructura administrativa. En el caso que al momento de realizar el Journaling
 una tabla no exista, deberá informar por archivo de log esta situación, pero el proceso deberá actualizar correctamente las tablas que sí existen.

 Cabe aclarar que esta operatoria en ningún momento hace una solicitud directa al FileSystem, la misma deberá manejarse siempre
 dentro de la memoria principal.
 * */

void insertAPI(char* input, Comando comando) {
	segmento_t *segmentoSeleccionado = NULL;

	uint16_t keyBuscada = atoi(comando.argumentos.INSERT.key); //TODO: se verifica que la key sea numerica?

	marco_t *marcoBuscado=NULL;
/*
	//Verifica si existe el segmento de la tabla en la memoria principal.
	if (verificarExistenciaSegmento(comando.argumentos.INSERT.nombreTabla, &segmentoSeleccionado)) {
		//Busca en sus páginas si contiene la key solicitada y de contenerla actualiza el valor insertando el Timestamp actual.
			if (contieneKey(segmentoSeleccionado, keyBuscada, &marcoBuscado)) {

				//En los request solo se utilizarán las comillas (“”)
				//para enmascarar el Value que se envíe. No se proveerán request con comillas en otros puntos.

				//TODO: ojo con pasarse del tamanio maximo para value
				//TODO: funcion que actue sobre el marco:
				//podria en lugar de tener un marco buscado, una pagina buscada y despues la mando a una funcion que entre al marco
				strcpy(marcoBuscado->value,comando.argumentos.INSERT.value);
				marcoBuscado->timestamp=getCurrentTime();
				printf("Se realizo el INSERT\n");
				printf("Muestro contenido memoria\n");
				mostrarContenidoMemoria();
			}
	}
*/
}

bool verificarExistenciaSegmento(char* nombreTabla, segmento_t ** segmentoAVerificar ) {
	char* pathSegmentoBuscado = malloc(
			sizeof(char) * strlen(nombreTabla) + strlen(pathLFS) + 1);
	strcpy(pathSegmentoBuscado, pathLFS);
	strcat(pathSegmentoBuscado, nombreTabla);

	//Recorro tabla de segmentos buscando pathSegmentoBuscado

	bool compararConPath(void* comparado) {
		//printf(RED"pathSegmentoBuscado: %s\npathComparado: %s"STD"\n",pathSegmentoBuscado,obtenerPath((segmento_t*)comparado));
		if (strcmp(pathSegmentoBuscado, obtenerPath((segmento_t*) comparado))) {
			return false;
		}
		return true;
	}

	t_list* listaConSegmentoBuscado = list_filter(tablaSegmentos.listaSegmentos,
			compararConPath);

	//Para ver que el path es el correcto

	//printf("El path del segmento buscado es: %s\n", pathSegmentoBuscado);
	free(pathSegmentoBuscado);

	if (list_is_empty(listaConSegmentoBuscado)) {
		printf(RED"APIMemoria.c: NO SE ENCONTRO EL SEGMENTO"STD"\n");
		return false;
	}
	//Tomo de la lista filtrada el segmento con el path coincidente

	*segmentoAVerificar = (segmento_t*) list_get(listaConSegmentoBuscado, 0);

	//Elimino la lista filtrada
	list_destroy(listaConSegmentoBuscado);

	return true;
}


//Busca en cada pagina de la tabla de paginas la referencia al marco y me fijo si coincide la key

//En vez de pasarle value le paso la operación que quiero hacer con value (modificarlo con insert o tomarlo con select)

bool contieneKey(segmento_t* segmentoElegido, uint16_t keyBuscada, pagina_t** paginaResultado) {
	//1. Tomo la tabla de paginas del segmento

	t_list * paginasDelSegmentoElegido = segmentoElegido->tablaPaginas->paginas;

	//2. Por cada pagina de la tabla, miro la referencia al marco
	//3. En cada marco me fijo si la key que tiene == keyBuscada
	//3.1 En caso de que la key sea la keyBuscada tomo el valor del value, y countUso++
	//3.2 Si la key NO es la buscada sigo con el siguiente marco

	bool compararConMarco(void* paginaComparada) {
		uint16_t *keyMarco=malloc(sizeof(uint16_t));
		memcpy(keyMarco,((pagina_t*) paginaComparada)->baseMarco + sizeof(timestamp_t),sizeof(uint16_t));
		if (*keyMarco == keyBuscada) {
			++((pagina_t*) paginaComparada)->countUso;
			free(keyMarco);
			return true;
		}
		free(keyMarco);
		return false;
	}

	t_list* listaConPaginaBuscada = list_filter(paginasDelSegmentoElegido,
			compararConMarco);

	if (list_is_empty(listaConPaginaBuscada)) {
		list_destroy(listaConPaginaBuscada);
		return false;
	}

	*paginaResultado = (pagina_t*) list_remove(listaConPaginaBuscada,0);
	list_destroy(listaConPaginaBuscada);

	return true;

}

char* obtenerValue(pagina_t *pagina){
	int sizeValue=(pagina->limiteMarco) - sizeof(timestamp_t)- sizeof(uint16_t);

	char*value=malloc(sizeof(char)*sizeValue);

	strcpy(value,((pagina->baseMarco) +sizeof(timestamp_t)+ sizeof(uint16_t) ));
	return value;
}