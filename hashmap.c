#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


#include <stdlib.h>
#include <string.h>
#include "hashmap.h"

void insertMap(HashMap * map, char * key, void * value) {
    if (map == NULL || key == NULL) return;

    long index = hash(key, map->capacity);

    for (long i = 0; i < map->capacity; i++) {
        long pos = (index + i) % map->capacity;

        Pair * pair = map->buckets[pos];

        // Si hay una clave repetida, no insertar
        if (pair != NULL && pair->key != NULL && is_equal(pair->key, key)) return;

        // Si está vacío o hay un espacio liberado (clave NULL), podemos insertar
        if (pair == NULL || pair->key == NULL) {
            // Crear nuevo par
            Pair * newPair = (Pair *)malloc(sizeof(Pair));
            if (newPair == NULL) return;

            newPair->key = key;
            newPair->value = value;

            map->buckets[pos] = newPair;
            map->size++;
            map->current = pos;
            return;
        }
    }
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)
    if (map == NULL) return;

    // a) Guardamos los buckets actuales
    Pair ** old_buckets = map->buckets;
    long old_capacity = map->capacity;

    // b) Duplicamos la capacidad
    map->capacity *= 2;

    // c) Asignamos un nuevo arreglo con la nueva capacidad
    map->buckets = (Pair **)malloc(sizeof(Pair *) * map->capacity);
    for (long i = 0; i < map->capacity; i++) {
        map->buckets[i] = NULL;
    }

    // d) Reinicializamos el size
    map->size = 0;

    // e) Insertamos los pares válidos nuevamente
    for (long i = 0; i < old_capacity; i++) {
        Pair * pair = old_buckets[i];
        if (pair != NULL && pair->key != NULL) {
            insertMap(map, pair->key, pair->value);
        }
    }

    // f) Liberamos el arreglo viejo, pero no los pares porque se están usando aún
    free(old_buckets);
}


HashMap * createMap(long capacity) {

    HashMap * map = (HashMap *)malloc(sizeof(HashMap));
    if (map == NULL) return NULL;


    map->buckets = (Pair **)malloc(sizeof(Pair *) * capacity);
    if (map->buckets == NULL) {
        free(map);
        return NULL;
    }

    for (long i = 0; i < capacity; i++) {
        map->buckets[i] = NULL;
    }


    map->size = 0;
    map->capacity = capacity;
    map->current = -1;

    return map;
}       

#include "hashmap.h"

void eraseMap(HashMap * map, char * key) {
    if (map == NULL || key == NULL) return;

    Pair * pair = searchMap(map, key);

    if (pair != NULL) {
        pair->key = NULL;  // Marcar como inválido
        map->size--;       // Disminuir el tamaño
    }
}
#include <stdlib.h>

#include "hashmap.h"
#include <string.h>

Pair * searchMap(HashMap * map, char * key) {
    if (map == NULL || key == NULL) return NULL;

    long index = hash(key, map->capacity);

    for (long i = 0; i < map->capacity; i++) {
        long pos = (index + i) % map->capacity;
        Pair * pair = map->buckets[pos];

        // Si la casilla está completamente vacía, no puede haber más adelante (no hay colisión)
        if (pair == NULL) return NULL;

        // Si hay clave válida y coincide con la buscada
        if (pair->key != NULL && is_equal(pair->key, key)) {
            map->current = pos;
            return pair;
        }
    }

    // No se encontró
    return NULL;
}


Pair * firstMap(HashMap * map) {
    if (map == NULL) return NULL;

    for (long i = 0; i < map->capacity; i++) {
        Pair * pair = map->buckets[i];
        if (pair != NULL && pair->key != NULL) {
            map->current = i;
            return pair;
        }
    }

    return NULL; // No hay pares válidos
}

Pair * nextMap(HashMap * map) {
    if (map == NULL) return NULL;

    for (long i = map->current + 1; i < map->capacity; i++) {
        Pair * pair = map->buckets[i];
        if (pair != NULL && pair->key != NULL) {
            map->current = i;
            return pair;
        }
    }

    return NULL; // No hay más pares válidos
}
