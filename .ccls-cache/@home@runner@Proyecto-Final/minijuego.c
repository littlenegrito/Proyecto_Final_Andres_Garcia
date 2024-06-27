#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tdas/stack.h"
#include "tdas/list.h"
#include "tdas/queue.h"
#include "tdas/extra.h"
#include "historia.h"
#include "minijuego.h"

Enemy* crearEnemigo(const char* name, unsigned short health, unsigned short strenght){
    Enemy* nuevoEnemigo = (Enemy*)malloc(sizeof(Enemy));
    if (!nuevoEnemigo) {
        printf("Error al asignar memoria para un nuevo enemigo.\n");
        exit(1);
    }
    nuevoEnemigo->name = strdup(name);
    nuevoEnemigo->health = health;
    nuevoEnemigo->strenght = strenght;
    return nuevoEnemigo;
}

void displayHealth(unsigned short playerHealth, unsigned short enemyHealth) {
  printf("\nJugador Vida: %d | Enemigo Vida: %d\n", playerHealth, enemyHealth);
}

void startCombat(Stack *actionStack) {
  printf("\n¡El combate ha comenzado!\n");
  stack_push(actionStack, "start");
}

void endCombat(Stack* actionStack) {
  printf("\nEl combate ha terminado.\n");
  stack_push(actionStack, "end");
}

void showOptions() {
  printf("\nOpciones:\n");
  printf("1. Atacar\n");
  printf("2. Usar Ítem\n");
  printf("3. Finalizar Combate\n");
  printf("Elija una opción: ");
}

void attack(unsigned short* playerHealth, unsigned short* enemyHealth) {
  int damage = rand() % 20 + 1;
  printf("\nHas atacado al enemigo y causado %d puntos de daño.\n", damage);
  *enemyHealth -= damage;
}

void useItem(unsigned short* playerHealth) {
  int heal = 50;
  printf("\nHas usado un ítem y recuperado %d puntos de vida.\n", heal);
  *playerHealth += heal;
}

void nextTurn(Stack* actionStack, unsigned short* playerHealth) {
  char* lastAction = stack_pop(actionStack);
  if (lastAction) {
      printf("\nTurno anterior: %s\n", lastAction);
      free(lastAction);
  }
  stack_push(actionStack, "next turn");

  // El enemigo ataca al jugador
  int enemyDamage = 20;
  printf("\nEl enemigo te ha atacado y causado %d puntos de daño.\n", enemyDamage);
  *playerHealth -= enemyDamage;
}

void announceWinner(unsigned short playerHealth, unsigned short enemyHealth) {
  if (enemyHealth <= 0) {
      printf("\n¡Has ganado!\n");
  } else if (playerHealth <= 0) {
      printf("\nHas muerto.\n");
  }
}

void iniciarBatalla(Player *jugador, Enemy *enemigo) {
    Stack* actionStack = stack_create();
    startCombat(actionStack);

    while (jugador->health > 0 && enemigo->health > 0) {
        displayHealth(jugador->health, enemigo->health);
        showOptions();

        int option;
        scanf("%d", &option);

        switch (option) {
            case 1:
                attack(&jugador->health, &enemigo->health);
                break;
            case 2:
                useItem(&jugador->health);
                break;
            case 3:
                endCombat(actionStack);
                return;
            default:
                printf("\nOpción inválida.\n");
                continue;
        }

        nextTurn(actionStack, &jugador->health);
        announceWinner(jugador->health, enemigo->health);
    }

    stack_clean(actionStack);
}