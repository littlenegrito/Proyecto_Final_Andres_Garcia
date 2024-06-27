#ifndef MINIJUEGO_H
#define MINIJUEGO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tdas/stack.h"
#include "tdas/list.h"
#include "tdas/queue.h"
#include "tdas/extra.h"
#include "historia.h"

Enemy* crearEnemigo(const char* name, unsigned short health, unsigned short strenght);

void displayHealth(unsigned short playerHealth, unsigned short enemyHealth);

void startCombat(Stack *actionStack);

void endCombat(Stack* actionStack);

void showOptions();

void attack(unsigned short* playerHealth, unsigned short* enemyHealth);

void useItem(unsigned short* playerHealth);

void nextTurn(Stack* actionStack, unsigned short* playerHealth);

void announceWinner(unsigned short playerHealth, unsigned short enemyHealth);

void iniciarBatalla(Player *jugador, Enemy *enemigo);

#endif /* MINIJUEGO_H */