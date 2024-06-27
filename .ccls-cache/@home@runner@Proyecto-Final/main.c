#include "historia.h"
#include "minijuego.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tdas/list.h"
#include "tdas/stack.h"
#include "tdas/queue.h"
#include "tdas/extra.h"


#define TOTALPOINTS 5
void mostrarEstadisticas(Player *stats, int mode){

  printf("\nLas estadísticas de %s son:\n", stats->name);
  printf("Fuerza: %d/10\n", stats->strenght);
  printf("Agilidad: %d/10\n", stats->agility);
  printf("Resistencia: %d/10\n", stats->endurance);
  printf("Carisma: %d/10\n", stats->charisma);
  printf("Suerte: %d/10\n", stats->luck);

  if(mode == 1){ // verificar tipo de mostrar
      printf("Dinero: %d\n", stats->gold);
      printf("Nivel Experiencia: %d\n", stats->level);
  }
  printf("\n");
  presioneTeclaParaContinuar();
}

int calcularAlturaActual(Nodo *nodoActual) {
    if (nodoActual == NULL) {
        return 0; // Si el nodo actual es NULL, la altura es 0
    } else {
        // Calcular la altura como la máxima profundidad entre los hijos
        int alturaMaxima = nodoActual->level;

        // Recorrer los hijos y calcular la altura máxima de los subárboles
        Nodo *hijoActual = nodoActual->firstChild;
        while (hijoActual != NULL) {
            int alturaHijo = calcularAlturaActual(hijoActual);
            if (alturaHijo > alturaMaxima) {
                alturaMaxima = alturaHijo;
            }
            hijoActual = hijoActual->nextSibling;
        }

        return alturaMaxima;
    }
}

void debugNodo(ArbolHistoria *arbol) {
    int id;
    printf("Ingrese el ID del nodo que desea debuggear: ");
    scanf("%d", &id);

    Nodo *nodo = buscarNodo(arbol->root, id);
    if (nodo != NULL) {
        printf("\nNodo encontrado:\n");
        printf("ID: %d\n\n", nodo->id);
        printf("Nombre: %s\n\n", nodo->name);
        printf("Descripción: %s\n", nodo->description);
        printf("Stats:\n");
        printf("  - Health: %d\n", nodo->stats.health);
        printf("  - Strenght: %d\n", nodo->stats.strenght);
        printf("  - Agility: %d\n", nodo->stats.agility);
        printf("  - Endurance: %d\n", nodo->stats.endurance);
        printf("  - Charisma: %d\n", nodo->stats.charisma);
        printf("  - Luck: %d\n", nodo->stats.luck);
        printf("  - Gold: %d\n", nodo->stats.gold);

        if(nodo->isMinigame){
            printf("\nEl nodo tiene un minijuego con enemigo %s.\n", nodo->enemy->name);
        }
        else printf("El nodo no tiene un minijuego.\n\n");

        // Mostrar condiciones del nodo si las hay
        if (nodo->condiciones != NULL) {
            printf("Condiciones requeridas:\n\n");
            Condicion *condicion = nodo->condiciones;
            while (condicion != NULL) {
                printf("  - Tipo: %s, Valor: %d\n", condicion->tipo, condicion->valor);
                condicion = condicion->next;
            }
        } else {
            printf("Este nodo no tiene condiciones requeridas.\n");
        }
    } else {
        printf("Nodo con ID %d no encontrado.\n", id);
    }
    printf("\n");
    presioneTeclaParaContinuar();
}

void maximizarEstadisticas(Player *jugador) {

    // Asignar los valores máximos a cada estadística
    jugador->health = 100;
    jugador->endurance = 10;
    jugador->strenght = 10;
    jugador->charisma = 10;
    jugador->agility = 10;
    jugador->luck = 10;
    jugador->gold = 9999;
    jugador->level = 10;
    printf("\nEstadísticas del jugador '%s' se han puesto al máximo.\n", jugador->name);
}

void menuDebug(ArbolHistoria *arbol, Player *player) {
    int opcion;
    do {
        printf("\n--- Menú de Depuración ---\n");
        printf("1. Buscar Nodo por ID\n");
        printf("2. Marcar Condiciones\n");
        printf("3. Ajustar Árbol\n");
        printf("4. Max Estadísticas\n");
        // Aquí puedes añadir más opciones de depuración
        printf("0. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                debugNodo(arbol);
                break;
            case 4:
                maximizarEstadisticas(player);
                mostrarEstadisticas(player, 1);
                break;
            case 0:
                printf("Saliendo del menú de depuración...\n");
                break;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
                break;
        }
    } while (opcion != 0);
}

void startStory(Player *player){
  printf("Bienvenido a la historia interactiva\nPara empezar, elige tu nombre: (Nombre Actual = %s):\n", player->name);
  char nombre[100];
  scanf(" %[^\n]", nombre);

    // Liberar la memoria anterior si ya estaba asignada
    if (player->name != NULL) {
        free(player->name);
    }
    // Asignar el nuevo nombre
    player->name = strdup(nombre);
    if (player->name == NULL) {
        printf("Error al asignar memoria para el nombre del jugador.\n");
        exit(1);
    }
    
  printf("\nHola %s, elige tus estadisticas:\n\n", nombre);

  int remPoints = TOTALPOINTS;

   while (remPoints > 0) {
          printf("Distribuye tus puntos entre las siguientes estadísticas:\n");
          printf("Tienes %d puntos restantes.\n\n", remPoints);
          printf("1. Fuerza (Actual: %d/10)\n", player->strenght);
          printf("2. Agilidad (Actual: %d/10)\n", player->agility);
          printf("3. Resistencia) (Actual: %d/10)\n", player->endurance);
          printf("4. Carisma (Actual: %d/10)\n", player->charisma);
          printf("5. Suerte (Actual: %d/10)\n", player->luck);
          printf("6. Información de Estadísticas\n\n");


          printf("Elige una opción: ");

          int opcion;
          scanf("%d", &opcion);
          if (opcion < 1 || opcion > 6) {
              printf("Opción no válida. Por favor, elige una opción entre 1 y 6.\n");
              continue;
          }
          if(opcion==6){
            printf("\nFuerza: Aumenta el daño que haces en combate y .\n");
            printf("Agilidad: Aumenta tu velocidad de movimiento y precisión en combate\n");
            printf("Resistencia: Aumenta tu salud máxima y reduce el daño que recibes de los enemigos.\n");
            printf("Carisma: Aumenta tus habilidades de persuasión y negociación\n");
            printf("Suerte: Aumenta tus probabilidades de encontrar objetos raros y mejores opciones\n\n");
            presioneTeclaParaContinuar();
            limpiarPantalla();
            continue;
          }

          printf("¿Cuántos puntos deseas asignar a esta estadística? (Máximo %d): ", remPoints);
          int points;
          scanf("%d", &points);

          if (points < 1 || points > remPoints) {
              printf("Cantidad no válida. Por favor, asigna entre 1 y %d puntos.\n", remPoints);
              continue;
          }

          switch (opcion) {
              case 1:
                  player->strenght += points;
                  break;
              case 2:
                  player->agility += points;
                  break;
              case 3:
                  player->endurance += points;
                  break;
              case 4:
                  player->charisma += points;
                  break;
              case 5:
                  player->luck += points;
                  break;
              default:
                  printf("Opción no válida. Por favor, elige una opción entre 1 y 6.\n");

          }
          remPoints -= points;
      }
      mostrarEstadisticas(player, 0);

}

void mostrarProgresoHistoria(ArbolHistoria *arbol, Nodo *nodoActual){
    int alturaTotal = calcularAlturaActual(arbol->root);
    printf("\n--- Progreso de la historia ---\n");
    printf("\nAltura actual en la historia: %d de %d niveles\n", nodoActual->level, alturaTotal);
    float porcentajeProgreso = ((float) nodoActual->level / (float)alturaTotal) * 100;
    printf("Llevas: %.2f%% completo\n", porcentajeProgreso);
}

Nodo* avanzarEvento(Nodo *nodoActual, Player *jugador, List *inv) {
    if (nodoActual == NULL) {
        printf("No hay más eventos.\n");
        return NULL;
    }
    printf("\n\n%s\n", nodoActual->description); // Mostrar evento actual

    // Verificar si el nodo tiene un minijuego y llamarlo si es así
    if (nodoActual->isMinigame) {
        printf("¡Has encontrado un minijuego!\n");
        //iniciarBatalla(jugador, nodoActual->enemy);
    }

    Nodo* hijoActual = nodoActual->firstChild; // Acceder al primer hijo
    if (hijoActual == NULL) {
        printf("Fin del camino. No hay más eventos disponibles.\n");
        return NULL;
    }
    int i = 1;
    Nodo* hijosValidos[10]; // Array para almacenar nodos hijos válidos
    int numHijosValidos = 0;

    while (hijoActual != NULL) { // Mostrar opciones del nodo actual
        if (verificarCondiciones(hijoActual, jugador)) { // Mostrar solo opciones válidas
            printf("%d. %s\n", i, hijoActual->name); // Mostrar el nombre de la opción
            hijosValidos[numHijosValidos++] = hijoActual; // Agregar hijo a los hijos válidos
            i++;
        }
        hijoActual = hijoActual->nextSibling;
    }

    if (numHijosValidos == 0) {
        printf("No hay opciones disponibles basadas en tus estadísticas.\n");
        return nodoActual;
    }

    int opcion;
    printf("\nSeleccione una opción: "); // seleccionar opción
    scanf("%d", &opcion);

    if (opcion < 1 || opcion > numHijosValidos) { // Verificación de entrada
        printf("Opción no válida. Intente nuevamente.\n");
        return nodoActual; // Reiniciar el proceso si la opción es inválida
    } else {
        return hijosValidos[opcion - 1]; // retornar el siguiente nodo hijo
    }
}

int main() {
    Player jugador; // personaje default
    jugador.name = strdup("Ignacio Araya");
    jugador.health = 100;
    jugador.endurance = 1;
    jugador.strenght = 1;
    jugador.charisma = 1;
    jugador.agility = 1;
    jugador.luck = 1;
    jugador.gold = 0;
    jugador.level = 1;
    
    //startStory(&jugador); // customizar personaje
    List* inventario = list_create(); // inventario
    addItem(inventario, "Espada Básica", "Una espada simple y eficaz.", 1);
    addItem(inventario, "Poción de Curación", "Restaura 50 puntos de vida.", 3);

    ArbolHistoria *arbol = (ArbolHistoria*)malloc(sizeof(ArbolHistoria));
    arbol->root = NULL;
    crearNodosHistoria(arbol, jugador, inventario);
    Nodo *nodoActual = buscarNodo(arbol->root, 1);
    
    int opcion;
    do {
        printf("\n--- Menú Principal ---\n");
        printf("1. Avanzar al Siguiente Evento\n");
        printf("2. Revisar Inventario\n");
        printf("3. Revisar Estadísticas\n");
        printf("4. Mostrar Progreso de la Historia\n");
        printf("5. Menú Debug (DESARROLLADOR)\n");
        printf("6. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        
        if (jugador.health <= 0) {
            printf("Has perdido tu vida\nGame Over...\n");
        } 
        switch (opcion) {
            case 1:
                nodoActual = avanzarEvento(nodoActual, &jugador, inventario);
                aplicarEfectos(nodoActual, &jugador, inventario);  // Aplicar efectos del nodo actual*/
                break;
            case 2:
                mostrarInventario(inventario);
                break;
            case 3:
                mostrarEstadisticas(&jugador, 1);
                break;
            case 4:
                mostrarProgresoHistoria(arbol, nodoActual);
                break;
            case 5:
                menuDebug(arbol, &jugador);
                break;
            case 6:
                printf("\nSaliendo del juego...\n");
                break;
            default:
                printf("\nOpción no válida. Intente nuevamente.\n");
                break;
        presioneTeclaParaContinuar();
        limpiarPantalla();
      }
    } while (opcion != 6);

    liberarArbol(arbol);
    return 0;
}
