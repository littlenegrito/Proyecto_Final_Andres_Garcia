#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tdas/list.h"
#include "tdas/stack.h"
#include "tdas/queue.h"
#include "tdas/extra.h"
#include "nodos.h"


// Crear nodo evento
struct Nodo* crearEvento(char *desc, Player stats, Minigame *minigame, int level) {
    Nodo *nuevo = (Nodo*)malloc(sizeof(Nodo));
    nuevo->description = strdup(desc);
    nuevo->stats = stats;
    nuevo->minigame = minigame;
    nuevo->level = level;
    nuevo->firstChild = NULL;
    return nuevo;
}
// Agrega nodo hijo
void agregarHijo(Nodo *parent, char *desc, Nodo *child) {
    NodoHijo *nuevo = (NodoHijo*)malloc(sizeof(NodoHijo));
    nuevo->description = strdup(desc);
    nuevo->child = child;
    nuevo->next = NULL;

    if (parent->firstChild == NULL) {
        parent->firstChild = nuevo;
    } else {
        NodoHijo *current = parent->firstChild; // recorrer lista de hijos
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = nuevo;
    }
}
