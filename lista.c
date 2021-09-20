#include "lista.h"
#include <stdlib.h>

typedef struct nodo{
    void* dato;
    struct nodo* siguiente;
} nodo_t;

nodo_t* crear_nodo(void* valor){
    nodo_t* nodo = malloc(sizeof(nodo_t));
    if (nodo == NULL){
        return NULL;
    }
    nodo->dato = valor;
    nodo->siguiente = NULL;
    return nodo;
}

struct lista{
    struct nodo* prim;
    struct nodo* ult;
    size_t largo;
};

lista_t *lista_crear(void){
    lista_t* lista = malloc(sizeof(lista_t));
    if (lista == NULL){
        return NULL;
    }
    lista->prim = NULL;
    lista->ult = NULL;
    lista->largo = 0;
    return lista;
}

bool lista_esta_vacia(const lista_t *lista){
    return (lista->largo == 0);
}

bool lista_insertar_primero(lista_t *lista, void *dato){
    nodo_t* nodo_nuevo = crear_nodo(dato);
    if (nodo_nuevo == NULL){
        return false;
    }
    if (lista_esta_vacia(lista)){
        lista->prim = nodo_nuevo;
        lista->ult = lista->prim;
        lista->largo ++;
        return true;
    }
    nodo_nuevo->siguiente = lista->prim;
    lista->prim = nodo_nuevo;
    lista->largo ++;
    return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato){
    nodo_t* nodo_nuevo = crear_nodo(dato);
    if (nodo_nuevo == NULL){
        return false;
    }
    if (lista_esta_vacia(lista)){
        lista->prim = nodo_nuevo;
        lista->ult = lista->prim;
        lista->largo ++;
        return true;                    
    }
    lista->ult->siguiente = nodo_nuevo;
    lista->ult = nodo_nuevo;
    lista->largo ++;
    return true;
}

void *lista_borrar_primero(lista_t *lista){
    if (lista_esta_vacia(lista)){
        return NULL;
    }
    lista->largo --;
    nodo_t* auxiliar = lista->prim;
    void* dato = lista->prim->dato;
    lista->prim = lista->prim->siguiente;
    free(auxiliar);
    return dato;
}

void *lista_ver_primero(const lista_t *lista){
    if (lista_esta_vacia(lista)){
        return NULL;
    }
    return (lista->prim->dato);
}

void *lista_ver_ultimo(const lista_t* lista){
    if (lista_esta_vacia(lista)){
        return NULL;
    }
    return (lista->ult->dato);
}

size_t lista_largo(const lista_t *lista){
    return (lista->largo);
}

void lista_destruir(lista_t *lista, void destruir_dato(void *)){ 
    while (!lista_esta_vacia(lista)){
        void* dato = lista_borrar_primero(lista);
        if (destruir_dato != NULL){
            destruir_dato(dato);
        }
    }
    free(lista);
}

struct lista_iter{
    lista_t* lista;
    nodo_t* anterior;
    nodo_t* actual;
};

lista_iter_t *lista_iter_crear(lista_t *lista){
    lista_iter_t* iter = malloc(sizeof(lista_iter_t));
    iter->lista = lista;
    iter->actual = lista->prim;
    iter->anterior = NULL;
    return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter){
    if (lista_iter_al_final(iter))  return false;
    iter->anterior = iter->actual;
    iter->actual = iter->actual->siguiente;
    return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter){
    if (lista_iter_al_final(iter))  return NULL;
    return (iter->actual->dato);
}

bool lista_iter_al_final(const lista_iter_t *iter){
    return (iter->actual == NULL);
}

void lista_iter_destruir(lista_iter_t *iter){
    free(iter);
}

bool lista_iter_insertar(lista_iter_t* iter, void* dato){
    nodo_t* nuevo_nodo = crear_nodo(dato);
    if (nuevo_nodo == NULL){
        return false;
    }
    if (lista_esta_vacia(iter->lista)){
        iter->lista->prim = nuevo_nodo;
        iter->lista->ult = iter->lista->prim;
    }

    if (iter->actual == iter->lista->prim && iter->actual != NULL){
        nuevo_nodo->siguiente = iter->lista->prim;
        iter->lista->prim = nuevo_nodo;
    }
    else if(!lista_esta_vacia(iter->lista)){
        iter->anterior->siguiente = nuevo_nodo;
        nuevo_nodo->siguiente = iter->actual;
    }
    if (lista_iter_al_final(iter) && !lista_esta_vacia(iter->lista)){
        iter->lista->ult->siguiente = nuevo_nodo;
        iter->lista->ult = nuevo_nodo;
    }
    iter->actual = nuevo_nodo;
    iter->lista->largo ++;
    return true;
}

void *lista_iter_borrar(lista_iter_t *iter){
    if (lista_iter_al_final(iter))  return NULL;
    nodo_t* auxiliar = iter->actual;
    void* dato = auxiliar->dato;
    if (iter->actual == iter->lista->prim){
        iter->lista->prim = iter->actual->siguiente;
    }
    else{
        iter->anterior->siguiente = iter->actual->siguiente;
    }
    if (iter->actual == iter->lista->ult){
        iter->lista->ult = iter->anterior;
    }
    iter->actual = iter->actual->siguiente;
    free(auxiliar);
    iter->lista->largo --;
    return dato;
}

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){
    nodo_t* actual = lista->prim;
    for (int i = 0; i < lista->largo; i++){
        if (!visitar(actual->dato, extra))  return;
        actual = actual->siguiente;
    }
}
