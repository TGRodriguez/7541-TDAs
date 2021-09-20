#include "cola.h"
#include <stdlib.h>

typedef struct nodo{
    void* dato;
    struct nodo* siguiente;
} nodo_t;

nodo_t* crear_nodo1(void* valor){
   nodo_t* nodo = malloc(sizeof(nodo_t));
    if (nodo == NULL){
        return NULL;
    }
    nodo->dato = valor;
    nodo->siguiente = NULL;
    return nodo;
}

struct cola{
    nodo_t* primero;
    nodo_t* ultimo;
};

cola_t* cola_crear(){
    cola_t* cola = malloc(sizeof(cola_t));
    if (cola == NULL){
        return NULL;
    }
    cola->primero = NULL;
    cola->ultimo = NULL;
    return cola;
}

void cola_destruir(cola_t *cola, void (*destruir_dato)(void*)){
    while (!cola_esta_vacia(cola)){
        void* dato = cola_desencolar(cola);
        if (destruir_dato != NULL){
            destruir_dato(dato);
        }
    }
    free(cola);
}

bool cola_esta_vacia(const cola_t *cola){
    return (cola->primero == NULL);
}

bool cola_encolar(cola_t *cola, void* valor){
    nodo_t* nodo_nuevo = crear_nodo1(valor);
    if (nodo_nuevo == NULL){
            return false;
    }
    if (cola_esta_vacia(cola)){
        cola->primero = nodo_nuevo; 
        cola->ultimo = cola->primero;
        return true;
    }
    cola->ultimo->siguiente = nodo_nuevo;
    cola->ultimo = nodo_nuevo;
    return true;
}

void* cola_ver_primero(const cola_t *cola){
    if (cola_esta_vacia(cola)){
        return NULL;
    }
    return (cola->primero->dato);
}

void* cola_desencolar(cola_t *cola){
    if (cola_esta_vacia(cola)){
        return NULL;
    }
    nodo_t* actual = cola->primero;
    void* dato = cola->primero->dato;
    cola->primero = cola->primero->siguiente;
    free(actual);
    return dato;   
}
