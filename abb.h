
#ifndef _ABB_H
#define _ABB_H

#include <stdbool.h>
#include <stddef.h>

// Función que compara dos claves.
typedef int (*abb_comparar_clave_t) (const char *, const char *);

// Función de destrucción de un dato de tipo void*.
typedef void (*abb_destruir_dato_t) (void *);

// El arbol binario de búsqueda está implementado como una estructura que almacena punteros 
// genéricos (datos) asociados a una clave de tipo char* 
struct abb;
typedef struct abb abb_t;

// Primitivas ABB

// Recibe la función de comparacion y destrucción y crea el abb.
// Postcondiciones: el abb fue creado.
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

// Precondiciones: el abb fue creado.
// Guarda en el abb la clave y con ella el dato asociado. Devuelve true si se pudo guardar, false en caso contrario.
// Postcondiciones: ahora la clave pertenece al abb, además se devolvió true, o false en caso de no haberse guardado.
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

// Precondiciones: el abb fue creado.
// Borra el dato asociado a la clave recibida por parámetro y lo devuelve. En caso de error o de no encontrar la clave, devuelve NULL
// Postcondiciones: se devolvió el dato o NULL si no estaba la clave.
void *abb_borrar(abb_t *arbol, const char *clave);

// Precondiciones: el abb fue creado.
// Devuelve el dato asociado a la clave recibida por parámetro. En caso de error o de no encontrar la clave, devuelve NULL
// Postcondiciones: se devolvió el dato o NULL si no estaba la clave.
void *abb_obtener(const abb_t *arbol, const char *clave);

// Precondiciones: el abb fue creado.
// Devuelve true en caso de que la clave recibica por parámetro se encontrara en el abb, false en caso contrario
// Postcondiciones: se devolvió true o false, si la clave estaba o no.
bool abb_pertenece(const abb_t *arbol, const char *clave);

// Precondiciones: el abb fue creado.
// Devuelve un size_t que representa la cantidad de elementos que el abb tiene guardado.
// Se devolvió la cantidad de elementos guardados en el abb.
size_t abb_cantidad(abb_t *arbol);

// Precondiciones: el abb fue creado.
// Destruye el abb y los datos que tenía guardados con la función de destrucción recibida en la creación
// Postcondiciones: se destruyó el abb.
void abb_destruir(abb_t *arbol);


// Iterador interno
// Precondiciones: el abb fue creado.
// Recorre en sentido in order el arbol y le aplica a cada elemento la función visitar. 
// La iteración se detiene cuando esta función devuelve false. Además recibe un parámetro extra.
// Postcondiciones: se recorrió in order el abb hasta el final o hasta que visitar devolvió false.
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);

// Iterador externo
typedef struct abb_iter abb_iter_t;

// Primitivas iterador externo

// Precondiciones: el abb fue creado
// Recibe un abb y crea un iterador externo.
// Postcondiciones: el iterador fue creado.
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Precondiciones: el iter fue creado.
// Avanza en el recorrido in order un elemento. Si pudo avanzar devuelve true, si estaba al final, false.
// Postcondiciones: el iter avanzó y se devolvio true, o false si no pudo avanzar.
bool abb_iter_in_avanzar(abb_iter_t *iter);

// Precondiciones: el iter fue creado.
// Devuelve la clave del elemento en donde el iterador estaba parado, NULL si estaba al final.
// Postcondiciones: se devolvió NULL o la clave del elemento donde está el iterador.
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

// Precondiciones: el iter fue creado.
// Devuelve true si el iterador está al final (una posición después del último elemento), false en caso contrario.
// Se devolvió true si el iterador estaba al final, false si no.
bool abb_iter_in_al_final(const abb_iter_t *iter);

// Precondiciones: el iter fue creado.
// Se destruyó el iterador
void abb_iter_in_destruir(abb_iter_t* iter);

// Pruebas del abb.
void pruebas_abb_alumno(void);

// Precondiciones: el abb fue creado.
// Itera el abb desde una clave hasta otra, ambas recibidas por parámetro.
// En cada una de las claves pertenecientes al intervalo recibido, se aplica la función visitar que indicia la condición de corte.
// También se recibe un extra.
// Postcondiciones: se recorrió el abb desde la clave 'inicio' hasta la clave 'fin'.
void iterar_desde_clave(abb_t* abb, char* inicio, char* fin, bool visitar(const char *, void *, void *), void *extra);

   
#endif // _ABB_H
