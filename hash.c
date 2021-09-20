#define  _POSIX_C_SOURCE 200809L
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"
#include "lista.h"

#define TAM_INICIAL 13
#define FACTOR_REDIMENSION 2
#define FACTOR_CARGA_MAX 1.5
#define FACTOR_CARGA_MIN 0.2



typedef struct hash_campo{ 
    char* clave;
    void* dato_hash;
    
}hash_campo_t;



hash_campo_t* crear_campo(const char* clave, void* dato_hash, hash_destruir_dato_t destruir_dato){
    hash_campo_t* campo = malloc(sizeof(hash_campo_t));
    if (!campo) return NULL;
    campo->clave = strdup(clave);
    if (!campo->clave){
        free(campo);
        return NULL;
    }
    campo->dato_hash = dato_hash;
    return campo;
}

void destruir_campo(hash_campo_t* campo, hash_destruir_dato_t destruir){
    if (campo->clave){
        if(destruir != NULL){ 
            destruir(campo->dato_hash);
        }    
        free(campo->clave);
        free(campo);
    }
}

/********************************************************/

struct hash{
    lista_t** tabla;
    size_t cant;
    size_t tam;
    hash_destruir_dato_t destruccion;
};


// Función djb2 por Dan Bernstein.
// http://www.cse.yorku.ca/~oz/hash.html
size_t funcion_hashing(const char *str){
    size_t hash = 5381;
    int c;
    while ((c = *str++)){
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

// Crea un hash auxiliar con un nuevo tamaño de tabla y lo devuelve.
hash_t* crear_hash_auxiliar(hash_t* hash, size_t nuevo_tam){
    hash_t* aux = malloc(sizeof(hash_t));
    if(!aux) return NULL;
    lista_t** tabla = calloc(sizeof(lista_t*), nuevo_tam);
    if (!tabla){
        free(aux);
        return NULL;
    }
    aux->tabla = tabla;
    aux->cant = 0;
    aux->tam = nuevo_tam;
    aux->destruccion = hash->destruccion;
    return aux;
}
// Recibe dos hash y actualiza los datos de uno para que sean iguales a los del segundo.
void hash_actualizar_datos(hash_t* hash, hash_t* hash_aux, size_t nuevo_tam){
    lista_t** tabla_a_liberar = hash->tabla;
    hash->tabla = hash_aux->tabla;
    hash->cant = hash_aux->cant;
    hash->tam = nuevo_tam;
    free(tabla_a_liberar);
    free(hash_aux);
}


// Función auxiliar para redimensionar el hash.
bool redimensionar_hash(hash_t* hash, size_t nuevo_tam){
    hash_t* hash_aux = crear_hash_auxiliar(hash, nuevo_tam);

    for (size_t i = 0; i < hash->tam; i++){
        if(!hash->tabla[i]) continue;
        lista_iter_t* iter = lista_iter_crear(hash->tabla[i]);
        if (!iter) return false;
        while(!lista_iter_al_final(iter)){
            hash_campo_t* actual = (hash_campo_t*) lista_iter_ver_actual(iter);
            const char* clave = actual->clave;
            hash_guardar(hash_aux, clave, actual->dato_hash);
            free(actual->clave);
            free(actual);
            lista_iter_avanzar(iter);
        }
        lista_iter_destruir(iter);  
        lista_destruir(hash->tabla[i], NULL);
    }
    hash_actualizar_datos(hash, hash_aux, nuevo_tam);
    return true;
}

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));
    if(!hash) return NULL;
    lista_t** tabla = calloc(sizeof(lista_t*), TAM_INICIAL);
    if(!tabla){
        free(hash);
        return NULL;
    }
    hash->tabla = tabla;
    hash->tam = TAM_INICIAL;
    hash->cant = 0;
    hash->destruccion = destruir_dato;
    return hash;
}

// Devuelve true si la clave se encuentra en la lista, false en caso contrario.
bool clave_en_lista(lista_iter_t* iter, const char* clave){
    while (!lista_iter_al_final(iter)){
        hash_campo_t* actual = (hash_campo_t*) lista_iter_ver_actual(iter);
        if(strcmp(actual->clave, clave) == 0){
            return true;
        }
        lista_iter_avanzar(iter);
    }
    return false;
}

// Devuelve false si existe la clave, y en caso contrario cambia el dato de la clave 
// y devuelve true.
bool insertado_repetido(lista_t* lista, lista_iter_t* iter, hash_campo_t* campo, hash_destruir_dato_t destruir){
    bool existe_clave_en_lista = clave_en_lista(iter, campo->clave);
    if (existe_clave_en_lista){
        hash_campo_t* actual = (hash_campo_t*) lista_iter_ver_actual(iter);
        void* dato_liberar = actual->dato_hash;
        actual->dato_hash = campo->dato_hash;
        if (destruir) destruir(dato_liberar);
    }
    return existe_clave_en_lista;
}

// Devuelve true si existe una lista en la posición de la clave, false en caso contrario.
bool clave_en_hash(const char* clave, const hash_t* hash, size_t pos){
    return (hash->tabla[pos] != NULL);
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    double indice_redimension = (double) hash->cant / (double) hash->tam;
    if(indice_redimension >= FACTOR_CARGA_MAX) {
        if(!redimensionar_hash(hash, hash->tam * FACTOR_REDIMENSION)) return false;
    }
    size_t pos = funcion_hashing(clave) % hash->tam;
    if (!clave_en_hash(clave, hash, pos)){
        hash->tabla[pos] = lista_crear();
        if (!hash->tabla[pos]) return false;
    }

    lista_iter_t* iter = lista_iter_crear(hash->tabla[pos]);
    hash_campo_t* campo = crear_campo(clave, dato, hash->destruccion);
    if (!campo) return false;
    if (!insertado_repetido(hash->tabla[pos], iter, campo, hash->destruccion)){
        lista_insertar_primero(hash->tabla[pos], campo);
        hash->cant ++;
    }else{
        free(campo->clave);
        free(campo);
    }
    lista_iter_destruir(iter);  
    return true;
}

// Borra la clave, destruye el campo y devuelve el dato de la clave. 
void* borrar_clave(lista_iter_t* iter, hash_t* hash){
    hash_campo_t* elem = lista_iter_ver_actual(iter);
    void* dato_devolver = elem->dato_hash;
    free(elem->clave);
    free(elem);
    lista_iter_borrar(iter);
    hash->cant --;
    return dato_devolver;

}

void *hash_borrar(hash_t *hash, const char *clave){
    double indice_redimension = (double) hash->cant / (double) hash->tam;
    if(indice_redimension < FACTOR_CARGA_MIN && hash->tam > TAM_INICIAL){ 
        if (!redimensionar_hash(hash, hash->tam / FACTOR_REDIMENSION)) return NULL;
    }
    size_t pos = funcion_hashing(clave) % hash->tam;
    if (!clave_en_hash(clave, hash, pos)) return NULL;

    lista_iter_t* iter = lista_iter_crear(hash->tabla[pos]);
    if (!iter) return NULL;
    void* dato = NULL;
    if (clave_en_lista(iter, clave)){ 
        dato = borrar_clave(iter, hash);
        if (lista_esta_vacia(hash->tabla[pos])){
            lista_destruir(hash->tabla[pos], NULL);
            hash->tabla[pos] = NULL; 
        } 
    }
    lista_iter_destruir(iter);
    return dato;
}

void *hash_obtener(const hash_t *hash, const char *clave){
    size_t pos = funcion_hashing(clave) % hash->tam;
    if (!clave_en_hash(clave, hash, pos)) return NULL;
    lista_iter_t* iter = lista_iter_crear(hash->tabla[pos]);
    if (!iter) return NULL;

    bool existe_clave_en_lista = clave_en_lista(iter, clave);
    hash_campo_t* actual = (hash_campo_t*) lista_iter_ver_actual(iter);
    lista_iter_destruir(iter);
    if (!existe_clave_en_lista) return NULL;
    return actual->dato_hash;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
    size_t pos = funcion_hashing(clave) % hash->tam;
    if (!clave_en_hash(clave, hash, pos)) return false;
    lista_iter_t* iter = lista_iter_crear(hash->tabla[pos]);
    if (!iter) return false;

    bool existe_clave_en_lista = clave_en_lista(iter, clave);
    lista_iter_destruir(iter);
    return existe_clave_en_lista;
}

size_t hash_cantidad(const hash_t *hash){
    return hash->cant;
}

void hash_destruir(hash_t *hash){
    for (size_t i = 0; i < hash->tam; i++){
        if (!hash->tabla[i]){
            continue;
        }
        while (!lista_esta_vacia(hash->tabla[i])){
            hash_campo_t* campo = lista_ver_primero(hash->tabla[i]);
            destruir_campo(campo, hash->destruccion);
            lista_borrar_primero(hash->tabla[i]);
        }
        lista_destruir(hash->tabla[i], NULL);
    }
    free(hash->tabla);
    free(hash);
}

struct hash_iter{
    const hash_t* hash;
    lista_iter_t* iter_lista;
    size_t pos_iter;
};

bool buscar_proxima_lista(hash_iter_t* iter){
    for (size_t i = iter->pos_iter; i < iter->hash->tam; i++){
        if (iter->hash->tabla[i] == NULL){
            iter->pos_iter += 1;
            continue;
        }
        iter->iter_lista = lista_iter_crear(iter->hash->tabla[i]);
        return true;
    }
    return false;
}

hash_iter_t *hash_iter_crear(const hash_t *hash){
    hash_iter_t* iter_hash = malloc(sizeof(hash_iter_t));
    if (!iter_hash) return NULL;
    iter_hash->pos_iter = 0;
    iter_hash->iter_lista = NULL;
    iter_hash->hash = hash;
    if(!buscar_proxima_lista(iter_hash)){ 
        iter_hash->pos_iter = hash->tam;
    }
    return iter_hash;
}

bool hash_iter_avanzar(hash_iter_t *iter){
    if(hash_iter_al_final(iter)) return false; 
    lista_iter_avanzar(iter->iter_lista);
    if (lista_iter_al_final(iter->iter_lista)){
        lista_iter_destruir(iter->iter_lista);
        iter->pos_iter ++;
        if (!buscar_proxima_lista(iter)){
            return false;
        }
    }
    return true; 

}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
    if (hash_iter_al_final(iter)) return NULL;
    hash_campo_t* campo = lista_iter_ver_actual(iter->iter_lista);
    return campo->clave;
}


bool hash_iter_al_final(const hash_iter_t *iter){
    return iter->pos_iter >= iter->hash->tam;
   
}

void hash_iter_destruir(hash_iter_t* iter){
    if(iter->iter_lista && iter->pos_iter != iter->hash->tam){
        lista_iter_destruir(iter->iter_lista);
    }
    free(iter);
}
