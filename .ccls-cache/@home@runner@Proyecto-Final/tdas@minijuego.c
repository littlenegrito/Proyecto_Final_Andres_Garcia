#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tdas/list.h"
#include "tdas/queue.h"
#include "tdas/extra.h"
#include "tdas/stack.h"
#include "historia.h"

void displayHealth(int playerHealth, int enemyHealth) {
  printf("\nJugador Vida: %d | Enemigo Vida: %d\n", playerHealth, enemyHealth);
}

void startCombat(Stack* actionStack) {
  printf("\n¡El combate ha comenzado!\n");
  push(actionStack, "start");
}

void endCombat(Stack* actionStack) {
  printf("\nEl combate ha terminado.\n");
  push(actionStack, "end");
}

void showOptions() {
  printf("\nOpciones:\n");
  printf("1. Atacar\n");
  printf("2. Usar Ítem\n");
  printf("3. Finalizar Combate\n");
  printf("Elija una opción: ");
}

void attack(int* playerHealth, int* enemyHealth) {
  int damage = rand() % 20 + 1;
  printf("\nHas atacado al enemigo y causado %d puntos de daño.\n", damage);
  *enemyHealth -= damage;
}

void useItem(int* playerHealth) {
  int heal = 50;
  printf("\nHas usado un ítem y recuperado %d puntos de vida.\n", heal);
  *playerHealth += heal;
}

void nextTurn(Stack* actionStack, int* playerHealth) {
  char* lastAction = pop(actionStack);
  if (lastAction) {
      printf("\nTurno anterior: %s\n", lastAction);
      free(lastAction);
  }
  push(actionStack, "next turn");

  // El enemigo ataca al jugador
  int enemyDamage = 20;
  printf("\nEl enemigo te ha atacado y causado %d puntos de daño.\n", enemyDamage);
  *playerHealth -= enemyDamage;
}

void announceWinner(int playerHealth, int enemyHealth) {
  if (enemyHealth <= 0) {
      printf("\n¡Has ganado!\n");
  } else if (playerHealth <= 0) {
      printf("\nHas muerto.\n");
  }
}

int main() {
  Stack actionStack = {NULL};
  int playerHealth = 100;
  int enemyHealth = 100;
  int option;

  startCombat(&actionStack);

  while (playerHealth > 0 && enemyHealth > 0) {
      displayHealth(playerHealth, enemyHealth);
      showOptions();
      scanf("%d", &option);

      switch (option) {
          case 1:
              attack(&playerHealth, &enemyHealth);
              break;
          case 2:
              useItem(&playerHealth);
              break;
          case 3:
              endCombat(&actionStack);
              return 0;
          default:
              printf("\nOpción inválida.\n");
              continue;
      }

      nextTurn(&actionStack, &playerHealth);
      announceWinner(playerHealth, enemyHealth);
  }
  return 0;
}