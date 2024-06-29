#ifndef HISTORIA_H
#define HISTORIA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tdas/list.h"
#include "tdas/map.h"
#include "tdas/stack.h"
#include "tdas/queue.h"
#include "tdas/extra.h"
#include "historia.c"


Nodo* crearEvento(int id, const char* nombre, const char* descripcion, Player stats, int level);
Nodo* crearEventoMinijuego(int id, const char* nombre, const char* descripcion, Player stats, Enemy *enemigo);
void agregarHijo(Nodo* nodoPadre, Nodo* nodoHijo);
Nodo* buscarNodoPorId(int id);
int compararHijos(Nodo* nodo1, Nodo* nodo2);
void consolidarNodos(Nodo* nodoPrincipal);
void liberarNodo(Nodo* nodo);
void liberarArbol(ArbolHistoria* arbol);
Condicion* crearCondicion(char *tipo, int valor);
void agregarCondicion(Nodo *hijo, char *tipo, int valor);
int verificarCondiciones(Nodo *hijo, Player *jugador);
void aplicarEfectos(Nodo *nodo, Player *jugador, List *inventario);

Enemy* crearEnemigo(const char* name, short health,  short strenght);
void displayHealth(Player *player, Enemy *enemy);
void endCombat(Stack* actionStack);
void showOptions();
void attack(Player *player, Enemy *enemy);
void useItem(Player *player, List *inventario);
void ataqueEnemigo(Player* player, Enemy* enemy);
void announceWinner(unsigned short playerHealth, unsigned short enemyHealth);
void iniciarBatalla(Player *jugador, Enemy *enemigo, List *inventario);


void crearNodosHistoria(ArbolHistoria *arbol, Player jugador, List *inventario);

#endif 