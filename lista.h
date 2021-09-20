#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdlib.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* La lista está planteada con punteros a los nodos inicial y final. */

struct lista;
typedef struct lista lista_t;

/* El iterador externo de la lista está planteado con punteros a los 
   nodos anterior y actual.                                          */

struct lista_iter;
typedef struct lista_iter lista_iter_t;

/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista.
// Post: se devolvió una nueva lista vacía. 
lista_t *lista_crear(void);

// Pre: la lista fue creada.
// Devuelve true si la lista no tiene elementos, o false en caso contrario.
bool lista_esta_vacia(const lista_t *lista);

// Pre: la lista fue creada.
// Inserta un elemento en el inicio de la lista. Devuelve false en caso de error.
// Post: se agregó un elemento al inicio de la lista. Dato es el primer elemento
// de la lista.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Pre: la lista fue creada.
// Inserta un elemento al final de la lista. Devuelve false en caso de error.
// Post: se agregó un elemento al final de la lista. Dato es el último elemento
// de la lista.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Pre: la lista fue creada.
// Elimina el primer elemento de la lista y devuelve su valor. Si la lista 
// estaba vacía, devuelve NULL.
// Post: se devolvió el valor del primer elemento anterior, la lista tiene
// un elemento menos, si no estaba vacía.
void *lista_borrar_primero(lista_t *lista);

// Pre: la lista fue creada.
// Devuelve el valor del primer elemento de la lista, o NULL si la lista
// estaba vacía.
// Post: se devolvió el valor del primer elemento de la lista, si no estaba 
// vacía.
void *lista_ver_primero(const lista_t *lista);

// Pre: la lista fue creada.
// Devuelve el valor del último elemento de la lista, o NULL si la lista
// estaba vacía.
// Post: se devolvió el valor del último elemento de la lista, si no estaba
// vacía.
void *lista_ver_ultimo(const lista_t* lista);

// Pre: la lista fue creada.
// Devuelve el largo de la lista.
size_t lista_largo(const lista_t *lista);

// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

// Crea un iterador que apunte al inicio de la lista.
// Post: se devolvió el iterador.
lista_iter_t *lista_iter_crear(lista_t *lista);

// Pre: el iterador fue creado.
// Avanza una posición de la iteración. Devuelve false si ya está en el final,
// o true en caso contrario.
// Post: se avanzó una posición de la iteración, si no estaba al final.
bool lista_iter_avanzar(lista_iter_t *iter);

// Pre: el iterador fue creado.
// Devuelve el valor del elemento actual del iterador.
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Pre: el iterador fue creado.
// Devuelve true si el iterador se encuentra en el final de la lista, es decir,
// cuando es NULL. Devuelve false en caso contrario.
bool lista_iter_al_final(const lista_iter_t *iter);

// Pre: el iterador fue creado.
// Destruye el iterador.
// Post: todos los elementos del iterador fueron destruidos.
void lista_iter_destruir(lista_iter_t *iter);

// Pre: el iterador fue creado.
// Inserta un elemento entre la posición actual del iterador y la anterior.
// Post: el nuevo actual es el elemento insertado.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Pre: el iterador fue creado.
// Borra el elemento actual del iterador.
// Post: el nuevo actual es el siguiente del actual anterior.
void *lista_iter_borrar(lista_iter_t *iter);



/* ******************************************************************
 *                    PRIMITIVA DEL ITERADOR INTERNO
 * *****************************************************************/

// Iterador interno de la lista. 
// Pre: la lista fue creada y existe una función visitar.
// Recorre la lista entera, o hasta que se cumpla una condición de corte
// en visitar, indicada por el extra.
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);


// Realiza las pruebas sobre la implementación de la lista.
void pruebas_lista_alumno(void);

#endif // LISTA_H
