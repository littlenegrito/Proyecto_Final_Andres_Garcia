#ifndef NODOS_H
#define NODOS_H

#include "tdas/list.h" 

typedef struct {
    char name[50];
    unsigned short strenght;       // Fuerza
    unsigned short agility;        // Agilidad
    unsigned short luck;           // Suerte
    unsigned short charisma;       // Carisma
    unsigned short endurance;      // Resistencia
    unsigned short health;         // Puntos Vida
    unsigned short gold;                      // 
    unsigned short level;

} Player;

typedef struct{
    char *name;
    unsigned short health;
    unsigned short strenght; 
    unsigned short agility; 
} Enemy;

typedef struct{
    char *name;        // Nombre del minijuego
    char *description;   // Descripción del minijuego
    void (*start)();    // Función que inicia el minijuego
    void (*end)();     // Función que maneja el resultado del minijuego
} Minigame;

typedef struct Nodo Nodo;

typedef struct NodoHijo{
    char *description; // Descripción de la opción correspondiente a este hijo
    Nodo *child;       // Puntero al nodo hijo
    struct NodoHijo *next; // Puntero al siguiente hijo en la lista
} NodoHijo;

struct Nodo{
    char *description;     // Descripción del evento o situación
    Player stats; // Estadísticas afectadas por este nodo
    Minigame *minigame;  // Puntero al minijuego (NULL si no hay minijuego)
    int level;             // Nivel o profundidad del nodo en el árbol
    NodoHijo *firstChild;  // Puntero al primer hijo en la lista de hijos
};

typedef struct {
    Nodo *root; // Puntero a la raíz del árbol
} Arbol;


Nodo *crearEvento(const char *descripcion, Player jugador, Nodo *parent, int depth);
void agregarHijo(Nodo *parent, const char *descripcion, Nodo *child);

#endif // NODOS_H