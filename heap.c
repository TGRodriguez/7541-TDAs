#include "heap.h"
#include <stdlib.h>
#define TAM_INICIAL 10
#define FACTOR_REDIMENSION 2

struct heap{
    void** datos;
    size_t tam;
    size_t cant;
    cmp_func_t comparar;
};

heap_t *heap_crear(cmp_func_t cmp){
    heap_t* heap = malloc(sizeof(heap_t));
    if (!heap) return NULL;
    heap->datos = malloc(sizeof(void*) * TAM_INICIAL);
    if (!heap->datos){
        free(heap);
        return NULL;
    }
    heap->tam = TAM_INICIAL;
    heap->cant = 0;
    heap->comparar = cmp;
    return heap;
}

// Redimensiona el heap, devuelve true si se redimensionó correctamente, false en caso contrario.
bool heap_redimensionar(heap_t* heap, size_t tam){
    void* datos_nuevos = realloc(heap->datos, sizeof(void*) * tam); 
    if (datos_nuevos == NULL){
        return false;
    }
    heap->tam = tam;
    heap->datos = datos_nuevos;
    return true;
}

size_t heap_cantidad(const heap_t *heap){
    return heap->cant;
}

bool heap_esta_vacio(const heap_t *heap){
    return !heap->cant;
}

void *heap_ver_max(const heap_t *heap){
    if (heap_esta_vacio(heap)) return NULL;
    return heap->datos[0];
}

// Devuelve la posición del padre de la posición pasada por parámetro.
size_t calcular_pos_padre(size_t pos){
    return (pos - 1) / 2;
}

// Devuelve la posición del hijo izquierdo.
size_t calcular_pos_hijo_izq(size_t pos){
    return pos * 2 + 1;
}

// Devuelve la posición del hijo derecho.
size_t calcular_pos_hijo_der(size_t pos){
    return pos * 2 + 2;
}

// Recibe dos posiciones del arreglo y devuelve true si la primera posición es mayor, 
// false en caso contrario.
bool es_mayor(void** arr, size_t pos_padre, size_t pos_hijo, cmp_func_t cmp){
    return cmp(arr[pos_padre], arr[pos_hijo]) > 0;
}

// Recibe dos posiciones del arreglo y swapea los elementos de dichas posiciones.
void swap(void** arr, size_t pos_uno, size_t pos_dos){
    void* elem = arr[pos_uno];
    arr[pos_uno] = arr[pos_dos];
    arr[pos_dos] = elem;
}

// Algoritmo recursivo que intercambia la posicion de un elemento con su padre si es más grande.
void upheap(void** arr, size_t pos_hijo, cmp_func_t cmp){
    if (pos_hijo == 0) return;
    size_t pos_padre = calcular_pos_padre(pos_hijo);
    if (!es_mayor(arr, pos_padre, pos_hijo, cmp)){
        swap(arr, pos_hijo, pos_padre);
        upheap(arr, pos_padre, cmp);
    }
}

bool heap_encolar(heap_t *heap, void *elem){
    if (heap->cant == heap->tam){
        if(!heap_redimensionar(heap, FACTOR_REDIMENSION * heap->tam)){
            return false;
        }  
    }
    heap->datos[heap->cant] = elem;
    upheap(heap->datos, heap->cant, heap->comparar);
    heap->cant ++;
    return true;
}

// Devuelve la posición del elemento mayor (entre los dos hijos y el padre).
size_t calcular_maximo(void** arr, size_t pos_padre, size_t pos_hijo_izq, size_t pos_hijo_der, cmp_func_t cmp, size_t cant){
    void* hijo_der = (pos_hijo_der < cant) ? arr[pos_hijo_der] : NULL;
    void* hijo_izq = (pos_hijo_izq < cant) ? arr[pos_hijo_izq] : NULL;
    size_t res = pos_padre;
    if(hijo_der){   // Si es un padre con dos hijos.
        if (es_mayor(arr, pos_hijo_izq, pos_hijo_der, cmp)){
            if (es_mayor(arr, pos_hijo_izq, pos_padre, cmp)) res = pos_hijo_izq;
        }else{
            if (es_mayor(arr, pos_hijo_der, pos_padre, cmp)) res = pos_hijo_der;
        }
    }else if (hijo_izq){ // Si sólo tiene un hijo. 
        if (es_mayor(arr, pos_hijo_izq, pos_padre, cmp)) res = pos_hijo_izq;
    }
    return res;
}

// Algoritmo recursivo que intercambia la posicion de un elemento con sus hijos si es mas chico que ellos.
void downheap(void** arr, size_t pos_padre, cmp_func_t cmp, size_t cant){
    if (pos_padre >= cant - 1) return;
    size_t maximo = calcular_maximo(arr, pos_padre, calcular_pos_hijo_izq(pos_padre), calcular_pos_hijo_der(pos_padre), cmp, cant);
    if (maximo != pos_padre){
        swap(arr, maximo, pos_padre);
        downheap(arr, maximo, cmp, cant);
    }
}

void *heap_desencolar(heap_t *heap){
    if (heap_esta_vacio(heap)) return NULL;
    heap->cant --;
    swap(heap->datos, 0, heap->cant);
    void* dato_anterior = heap->datos[heap->cant];
    downheap(heap->datos, 0, heap->comparar, heap->cant);
    if (heap->cant > TAM_INICIAL && heap->cant == heap->tam / FACTOR_REDIMENSION * FACTOR_REDIMENSION){
        heap_redimensionar(heap, heap->tam / FACTOR_REDIMENSION);
    }
    return dato_anterior;
}

void heap_destruir(heap_t *heap, void (*destruir_elemento)(void *e)){
    if (destruir_elemento) {
	    for (int i = 0; i < heap->cant; i++) {
			destruir_elemento(heap->datos[i]);
		}
	}
	free(heap->datos);
	free(heap);
}

// Algoritmo de Heapify, aplica downheap desde el último elemento que no es una hoja hasta el primero.
void heapify(void** datos, size_t n, cmp_func_t cmp){
    for(size_t i = n/2 - 1; i > 0; i--){
        downheap(datos, i, cmp, n);
    }
    downheap(datos, 0, cmp, n);
}

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){
    heap_t* heap = malloc(sizeof(heap_t));
    if(!heap) return NULL;
    void** datos = malloc(sizeof(void*) * n);
    if(!datos){
        free(heap);
        return NULL;
    }
    heap->datos = datos;
    for(size_t i = 0; i < n; i++){
        heap->datos[i] = arreglo[i];
    }
    heapify(heap->datos, n, cmp);
    heap->tam = n;
    heap->cant = n;
    heap->comparar = cmp;
    return heap;
}

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){
    heapify(elementos, cant, cmp);
    for(size_t i = cant - 1; i > 0; i--){
        swap(elementos, 0, i);
        downheap(elementos, 0, cmp, i);
    }
}
