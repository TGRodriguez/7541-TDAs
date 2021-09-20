#define _POSIX_C_SOURCE 200809L 
#include "strutil.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *substr(const char *str, size_t n){
    char* res = calloc(n + 1, sizeof(char));
    if (!res){
        return NULL;
    }
    return strncpy(res, str, n);
}

char **split(const char *str, char sep){
    size_t len = strlen(str);
    size_t cantidad_separadores = 0;

    for (size_t i = 0; i < len; i ++){
        if (str[i] == sep){
            cantidad_separadores += 1;
        }
    }

    char** res = calloc((cantidad_separadores + 2), sizeof(char*));
    if (res == NULL){
        return NULL;
    }
    res[cantidad_separadores + 1] = NULL;

    if (!cantidad_separadores){
        res[0] = strdup(str);
        return res;
    }

    size_t posiciones_separadores[cantidad_separadores];
    size_t contador = 0;

    for (size_t i = 0; i < len; i ++){
        if (str[i] == sep){
            posiciones_separadores[contador] = i;
            contador += 1;
        }
    }

    res[0] = substr(str, posiciones_separadores[0]);
    res[cantidad_separadores] = substr(str + posiciones_separadores[cantidad_separadores - 1] + 1, len - 1);
    
    for (size_t i = 1; i < cantidad_separadores; i++){
        res[i] = substr(str + posiciones_separadores[i - 1] + 1, posiciones_separadores[i] - posiciones_separadores[i - 1] - 1);
    }
    return res;
}

char *join(char **strv, char sep){
    if (strv == NULL || strv[0] == NULL){
        char* res = calloc(1, sizeof(char));
        if (res == NULL) return NULL;
        return res;
    }
    size_t cantidad_separadores = 0;
    size_t len = 0;

    for (size_t i = 1; strv[i]; i ++){
        cantidad_separadores += 1;
        len += strlen(strv[i]);
    }

    len += strlen(strv[0]) + cantidad_separadores;
    char* res = calloc(len + 1, sizeof(char));

    if (res == NULL){
        return NULL;
    }
    
    strcpy(res, strv[0]);

    if (!cantidad_separadores){
        return res;
    }

    size_t len_str[cantidad_separadores + 1];
    len_str[0] = strlen(res);
    for (size_t i = 1; strv[i]; i++){
        len_str[i] = strlen(strv[i]);
    }
    res[len_str[0]] = sep;
    size_t contador = len_str[0];
    if (!sep) len -= cantidad_separadores;
    for (size_t i = 1; i < cantidad_separadores; i++){
        if (!sep) contador -= 1;
        strcat(res + contador + 1, strv[i]);
        contador += len_str[i] + 1;
        res[contador] = sep;
    }
    strcat(res + (len - len_str[cantidad_separadores]), strv[cantidad_separadores]);
    return res;
}


void free_strv(char *strv[]){
    for (size_t i = 0; strv[i]; i++){
        free(strv[i]);
    }
    free(strv);
}
