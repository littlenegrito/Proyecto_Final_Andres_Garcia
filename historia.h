#ifndef HISTORIA_H
#define HISTORIA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tdas/list.h"
#include "tdas/stack.h"
#include "tdas/queue.h"
#include "tdas/extra.h"

typedef struct Player{
    char *name;
    unsigned short strenght;       // Fuerza
    unsigned short agility;        // Agilidad
    unsigned short luck;           // Suerte
    unsigned short charisma;       // Carisma
    unsigned short endurance;      // Resistencia
    unsigned short health;         // Puntos Vida
    unsigned short gold;           // Dinero
    unsigned short level;

} Player;

typedef struct Enemy{
    char *name;
    unsigned short health;
    unsigned short strenght; 
} Enemy;

typedef struct Condicion {
    char *tipo;         // Tipo de condición (por ejemplo, "carisma", "fuerza", etc.)
    int valor;          // Valor requerido para la condición
    struct Condicion *next; // Puntero a la siguiente condición
} Condicion;

typedef struct Nodo {
    int id;
    char *name;
    char *description;
    Player stats;
    int level;
    int isMinigame;
    Enemy *enemy;
    Condicion *condiciones; // Lista de condiciones para acceder a este nodo hijo
    struct Nodo *firstChild; // Primer hijo del nodo
    struct Nodo *nextSibling; // Hermano derecho del nodo
} Nodo;

typedef struct {
    Nodo *root;
} ArbolHistoria;

Nodo* crearEvento(int id, const char* nombre, const char* descripcion, Player stats, int level);
Nodo* crearEventoMinijuego(int id, const char* nombre, const char* descripcion, Player stats, Enemy *enemigo);
void agregarHijo(Nodo* nodoPadre, Nodo* nodoHijo);
Nodo* buscarNodo(Nodo* raiz, int id);
void liberarNodo(Nodo* nodo);
void liberarArbol(ArbolHistoria* arbol);
Condicion* crearCondicion(char *tipo, int valor);
void agregarCondicion(Nodo *hijo, char *tipo, int valor);
int verificarCondiciones(Nodo *hijo, Player *jugador);
void aplicarEfectos(Nodo *nodo, Player *jugador, List *inventario);
void crearNodosHistoria(ArbolHistoria *arbol, Player jugador, List *inventario);

#endif 