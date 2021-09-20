#include "pila.h"
#include <stdlib.h>
#define TAM_INICIAL 1 //Capacidad inicial de la pila.

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;   // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

// ...

pila_t* pila_crear(void){
    pila_t* pila = malloc(sizeof(pila_t));
    if (pila == NULL){
        return NULL;
    }
    pila->datos = malloc(sizeof(void*) * TAM_INICIAL); 
    if (pila->datos == NULL){  
        free(pila);
        return NULL;
    }
    pila->cantidad = 0;
    pila->capacidad = TAM_INICIAL; 
    return pila;
}

void pila_destruir(pila_t *pila){
    free(pila->datos);
    free(pila);
}

bool pila_esta_vacia(const pila_t *pila){
    return (pila->cantidad == 0);
}

bool pila_redimensionar(pila_t* pila, size_t tam){
    void* datos_nuevos = realloc(pila->datos, sizeof(void*) * tam); 
    if (datos_nuevos == NULL){
        return false;
    }
    pila->capacidad = tam;
    pila->datos = datos_nuevos;
    return true;
}

bool pila_necesita_agrandarse(pila_t* pila){
    if (pila->cantidad == pila->capacidad){
        return true;
    }
    return false;
}

bool pila_apilar(pila_t *pila, void* valor){
    pila->datos[pila->cantidad] = valor;
    pila->cantidad++;
    if (pila_necesita_agrandarse(pila)){
        pila_redimensionar(pila, pila->capacidad * 2);
    }
    return true;
}

void* pila_ver_tope(const pila_t *pila){
    if (pila_esta_vacia(pila)){
        return NULL;
    }
    return pila->datos[pila->cantidad - 1];
}

bool pila_necesita_achicarse(pila_t* pila){
    if (pila->cantidad * 4 <= pila->capacidad){
        return true;
    }
    return false;
}

void* pila_desapilar(pila_t *pila){ 
    if (pila_esta_vacia(pila)){
        return NULL;
    }
    pila->cantidad--;
    if (pila_necesita_achicarse(pila)){
        pila_redimensionar(pila, pila->capacidad / 2);
    }
    return pila->datos[pila->cantidad];
}
