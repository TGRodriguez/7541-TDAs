#define  _POSIX_C_SOURCE 200809L
#include <string.h>
#include "abb.h"
#include "pila.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct nodo_abb{
    struct nodo_abb* izq;
    struct nodo_abb* der;
    char* clave;
    void* dato;
}nodo_abb_t;
 
nodo_abb_t* crear_nodo_abb(const char* clave, void* dato){
    nodo_abb_t* nodo = malloc(sizeof(nodo_abb_t));
    if (!nodo) return NULL;
    nodo->izq = NULL;
    nodo->der = NULL;
    nodo->clave = strdup(clave);
    if (!nodo->clave){
        free(nodo);
        return NULL;
    }
    nodo->dato = dato;
    return nodo;
}

struct abb{
    nodo_abb_t* raiz;
    size_t cant;
    abb_destruir_dato_t destruir;
    abb_comparar_clave_t comparar;
};

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
    abb_t* abb = malloc(sizeof(abb_t));
    if (!abb) return NULL;
    abb->raiz = NULL;
    abb->cant = 0;
    abb->destruir = destruir_dato;
    abb->comparar = cmp;
    return abb;
}

// Destruye el nodo y devuelve su dato.
void* destruir_nodo(nodo_abb_t* nodo){
    void* dato = nodo->dato;
    free(nodo->clave); 
    free(nodo);
    return dato;
}

size_t abb_cantidad(abb_t *arbol){
    return arbol->cant;
}

nodo_abb_t* buscar_nodo(abb_comparar_clave_t cmp, nodo_abb_t* nodo, const char* clave){
    if (!nodo) return NULL;
    int comparacion = cmp(nodo->clave, clave);
    if (comparacion == 0){
        return nodo;
    }
    if (comparacion > 0){
        return buscar_nodo(cmp, nodo->izq, clave);
    }
    return buscar_nodo(cmp, nodo->der, clave);
}

// Devuelve el padre del nodo, o NULL en caso de que no haya raíz.
nodo_abb_t* buscar_padre(abb_comparar_clave_t cmp, nodo_abb_t *nodo, const char *clave){
    if (!nodo) return NULL;
    int comparacion = cmp(nodo->clave, clave);
    if (comparacion < 0){
        if(!nodo->der || cmp(nodo->der->clave,clave) == 0){return nodo;}
        return buscar_padre(cmp, nodo->der, clave);
    }
    if (comparacion > 0){
        if(!nodo->izq || cmp(nodo->izq->clave,clave) == 0){return nodo;}
        return buscar_padre(cmp, nodo->izq, clave);
    }
    return nodo; 
}

// Reemplaza el dato en un nodo, borrando el dato anterior si es que el árbol tiene función de destrucción.
void reemplazar_dato(nodo_abb_t* nodo, abb_destruir_dato_t destruir, nodo_abb_t* nuevo_nodo){
    void* dato = nodo->dato;
    if (destruir) destruir(dato);
    nodo->dato = destruir_nodo(nuevo_nodo);
}

// Actualiza al hijo derecho o izquierdo del padre, o a la raíz del árbol.
void actualizar_nodo(nodo_abb_t* nodo, abb_t* abb, nodo_abb_t* nuevo_nodo, size_t pos){
    if (pos == 0){
        nodo->izq = nuevo_nodo;
    }
    if (pos == 1){
        abb->raiz = nuevo_nodo;
    }
    if (pos == 2){
        nodo->der = nuevo_nodo;
    }
    abb->cant ++;
}

// Guarda la nueva hoja (si es que la clave nueva no estaba en el arbol) actualizando el nodo del padre.
// Si la clave ya existía, actualiza su dato y destruye el nuevo nodo (El reemplazo es en el nodo ya existente).
void guardar_hoja(nodo_abb_t* padre, nodo_abb_t* nuevo_nodo, abb_t* arbol, const char* clave){
    int comparacion = arbol->comparar(padre->clave, clave);
    if (comparacion < 0){
        if (!padre->der){
            actualizar_nodo(padre, arbol, nuevo_nodo, 2);
        }else{
            reemplazar_dato(padre->der, arbol->destruir, nuevo_nodo);
        }
    }else{ 
        if (!padre->izq){
            actualizar_nodo(padre, arbol, nuevo_nodo, 0);
        }else{
            reemplazar_dato(padre->izq, arbol->destruir, nuevo_nodo);
        }
    }
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
    nodo_abb_t* nuevo_nodo = crear_nodo_abb(clave, dato);
    if (!nuevo_nodo) return false;
    nodo_abb_t* padre = buscar_padre(arbol->comparar, arbol->raiz, clave);
    if (!padre){
        actualizar_nodo(arbol->raiz, arbol, nuevo_nodo, 1);
    }
    else if (!strcmp(arbol->raiz->clave, clave)){
        reemplazar_dato(arbol->raiz, arbol->destruir, nuevo_nodo);
    }else{
        guardar_hoja(padre, nuevo_nodo, arbol, clave);
    }
    return true;
}

void* abb_obtener(const abb_t* arbol, const char* clave){
    nodo_abb_t* nodo = buscar_nodo(arbol->comparar, arbol->raiz, clave);
    if(!nodo) return NULL;
    return nodo->dato;
}

bool abb_pertenece(const abb_t* arbol, const char* clave){
    nodo_abb_t* nodo = buscar_nodo(arbol->comparar, arbol->raiz, clave);
    return (nodo != NULL);
}

/*FUNCIONES AUXILIARES PARA BORRAR UN NODO*/

// Devuelve la cantidad de hijos del nodo.
int calcular_hijos(nodo_abb_t* nodo){
    int res = 0;
    if(nodo->izq) res += 1;
    if(nodo->der) res += 1;
    return res;
}

// Pre: El nodo tiene uno o ningún hijo.
// Devuelve algún hijo (NULL si no tiene ninguno).
nodo_abb_t* buscar_hijo(nodo_abb_t* nodo){
    if (!nodo->izq) return nodo->der;
    return nodo->izq;
}

// Devuelve el nodo reemplazante para el caso de borrar con dos hijos.
nodo_abb_t* buscar_reemplazante(nodo_abb_t* nodo){
    nodo_abb_t* actual = nodo;
    if (actual) actual = actual->der;
    while (actual && actual->izq){ 
        actual = actual->izq;
    }
    return actual;
}

// Caso de borrar en el que el nodo no tiene hijos.
void* borrar_hoja(abb_t* abb, nodo_abb_t* nodo, nodo_abb_t* padre, int comparacion){
    //int comparacion = abb->comparar(nodo->clave, padre->clave);
    if (comparacion == 0){
        abb->raiz = buscar_hijo(nodo);
    }else{
        if (comparacion > 0){
            padre->der = NULL;
        }else{
            padre->izq = NULL;
        }
    }
    abb->cant --;
    return destruir_nodo(nodo);
}

// Caso de borrar en el que el nodo tiene un hijo.
void* borrar_con_un_hijo(abb_t* abb, nodo_abb_t* nodo, nodo_abb_t* padre, int comparacion){
    if (comparacion == 0){ 
        abb->raiz = buscar_hijo(nodo);
    }else{
        if (comparacion > 0){ 
            padre->der = buscar_hijo(nodo);
        }else{
            padre->izq = buscar_hijo(nodo);
        }
    }
    abb->cant --;
    return destruir_nodo(nodo);
}

// Caso de borrar en el que el nodo tiene dos hijos.
void* borrar_con_dos_hijos(abb_t* abb, nodo_abb_t* nodo, nodo_abb_t* padre){
    void* dato_anterior = nodo->dato;
    nodo_abb_t* reemplazante = buscar_reemplazante(nodo);
    char* clave_reemplazante = strdup(reemplazante->clave);
    void* dato_aux = abb_borrar(abb, reemplazante->clave);
    free(nodo->clave);
    nodo->clave = clave_reemplazante;
    nodo->dato = dato_aux;
    return dato_anterior;
}

void *abb_borrar(abb_t *arbol, const char *clave){
    nodo_abb_t* nodo = buscar_nodo(arbol->comparar, arbol->raiz, clave);
    if (!nodo) return NULL;

    int cantidad_hijos = calcular_hijos(nodo);
    nodo_abb_t* padre = buscar_padre(arbol->comparar, arbol->raiz, clave);
    void* dato;
    
    if (cantidad_hijos == 2){
        dato = borrar_con_dos_hijos(arbol, nodo, padre);
    }
    else{
        int comparacion = arbol->comparar(nodo->clave, padre->clave);
        if (cantidad_hijos == 0){    
            dato = borrar_hoja(arbol, nodo, padre, comparacion);
        }
        if (cantidad_hijos == 1){ // Es un if trivial para que sea más legible, sería un else.
            dato = borrar_con_un_hijo(arbol, nodo, padre, comparacion);
        }
    }
    return dato;
}

// Funcion auxiliar recursiva para abb_destruir(). Si la funcion de destruccion no es NULL, se usa para destruir el dato.
void destruir_recursivo(abb_destruir_dato_t destruccion, nodo_abb_t* nodo){
    if(!nodo) return;

    destruir_recursivo(destruccion, nodo->izq);
    destruir_recursivo(destruccion, nodo->der);
    
    void* dato = destruir_nodo(nodo);
    if(destruccion) destruccion(dato);
} 

void abb_destruir(abb_t* arbol){
    destruir_recursivo(arbol->destruir, arbol->raiz);
    free(arbol);
}

// Funcion recursiva para iterar in-order el abb. 
void rec_abb_in_order(nodo_abb_t* nodo, bool visitar(const char *, void *, void *), void *extra, int* error){
    if (!nodo || !*error) return;
    rec_abb_in_order(nodo->izq, visitar, extra, error);
    if (*error && !visitar(nodo->clave, nodo->dato, extra)) {
        *error = 0;
        return;
    }
    rec_abb_in_order(nodo->der, visitar, extra, error);
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra){
    int ok = 1;
    rec_abb_in_order(arbol->raiz, visitar, extra, &ok);
} 

struct abb_iter{
    pila_t* pila;
};

// Recibe una pila y un nodo y apila a todos los hijos izquierdos incluyendo al nodo.
void apilar_hijos_izquierdos(nodo_abb_t* nodo, pila_t* pila){
    if (!nodo) return;
    nodo_abb_t* actual = nodo;
    pila_apilar(pila, actual);
    while(actual && actual->izq){
        pila_apilar(pila, actual->izq);
        actual = actual->izq;
    }
}

abb_iter_t *abb_iter_in_crear(const abb_t *arbol){
    abb_iter_t* iter = malloc(sizeof(abb_iter_t));
    if (!iter) return NULL;
    pila_t* pila = pila_crear();
    if (!pila){
        free(iter);
        return NULL;
    }
    iter->pila = pila;
    apilar_hijos_izquierdos(arbol->raiz, pila);
    return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter){
    if (pila_esta_vacia(iter->pila)) return false;
    nodo_abb_t* desapilado = pila_desapilar(iter->pila);
    if (desapilado->der){
        apilar_hijos_izquierdos(desapilado->der, iter->pila);
    }
    return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
    nodo_abb_t* nodo = pila_ver_tope(iter->pila);
    if (!nodo) return NULL;
    return nodo->clave;

}

bool abb_iter_in_al_final(const abb_iter_t *iter){
    return pila_esta_vacia(iter->pila);
}

void abb_iter_in_destruir(abb_iter_t* iter){
    pila_destruir(iter->pila);
    free(iter);
}


void _iterar_desde_clave(nodo_abb_t* nodo, abb_comparar_clave_t cmp, char* inicio, char* fin, bool visitar(const char *, void *, void *), void *extra, size_t* error){
    if (!nodo || !*error) return;
    if (cmp(inicio, nodo->clave) < 0){
        _iterar_desde_clave(nodo->izq, cmp, inicio, fin, visitar, extra, error);
    }
    if ((!strcmp(inicio, "\0") || (cmp(inicio, nodo->clave) <= 0)) && ((cmp(fin, nodo->clave) >= 0) || !strcmp(fin, "\0"))){
        if (*error && !visitar(nodo->clave, nodo->dato, extra)){
            *error = 0;
            return;
        }
    }
    if (cmp(fin, nodo->clave) > 0 || !strcmp(fin, "\0")){
        _iterar_desde_clave(nodo->der, cmp, inicio, fin, visitar, extra, error);
    }
}

void iterar_desde_clave(abb_t* abb, char* inicio, char* fin, bool visitar(const char *, void *, void *), void *extra){
    size_t ok = 1;
    _iterar_desde_clave(abb->raiz, abb->comparar, inicio, fin, visitar, extra, &ok);
}
