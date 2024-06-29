#include "historia.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tdas/list.h"
#include "tdas/map.h"
#include "tdas/set.h"
#include "tdas/stack.h"
#include "tdas/queue.h"
#include "tdas/extra.h"
Map* todosLosNodos = NULL; // Declaración global
int numTotalNodos = 0; // Declaración global

#define TOTALPOINTS 5
void mostrarEstadisticas(Player *stats, int mode){

  printf("\nLas estadísticas de %s son:\n", stats->name);
  printf("Vida: %d\n", stats->health);
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
int compararIds(void* id1, void* id2) {
    return (*(int*)id1 == *(int*)id2);
}
void debugNodo() {
    int id;
    printf("Ingrese el ID del nodo que desea debuggear: ");
    scanf("%d", &id);

    Nodo *nodo = buscarNodoPorId(id);
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
void mostrarIdTodosNodos() {
    if (todosLosNodos == NULL) {
        printf("Conjunto de nodos no inicializado.\n");
        return;
    }
    printf("Total Nodos: %d\nIDs de todos los nodos:\n\n", numTotalNodos);

    // Itera desde 1 hasta el número total de nodos
    for (int i = 1; i <= numTotalNodos; i++) {
        // Busca el nodo por su ID usando el contador global
        void *idNodo = (void *)&i;
        Nodo *nodo = (Nodo *)set_search(todosLosNodos, idNodo);

        if (nodo != NULL) {
            printf("- ID: %d\n", nodo->id);
        } else {
            printf("- ID: %d (No encontrado)\n", i);
        }
    }
}
// Función recursiva para calcular la altura máxima del árbol
int calcularAlturaRecursivo(Nodo *nodo) {
    if (nodo == NULL) {
        return 0; // Si el nodo es nulo, altura es 0
    }

    // Calcular la altura como 1 (el nodo actual) más la máxima altura de los hijos
    int alturaMaxima = 1; // Comenzamos con el nivel del nodo actual

    // Recorrer los hijos y calcular la altura máxima de los subárboles
    Nodo *hijoActual = nodo->firstChild;
    while (hijoActual != NULL) {
        int alturaHijo = 1 + calcularAlturaRecursivo(hijoActual);
        if (alturaHijo > alturaMaxima) {
            alturaMaxima = alturaHijo;
        }
        hijoActual = hijoActual->nextSibling;
    }

    return alturaMaxima;
}
// Función para calcular la altura máxima del árbol
int calcularAlturaArbol(ArbolHistoria *arbol) {
    if (arbol == NULL || arbol->root == NULL) {
        return 0; // Si el árbol o la raíz son nulos, altura es 0
    }

    // Usaremos una función auxiliar para calcular la altura recursivamente
    return calcularAlturaRecursivo(arbol->root);
}
void menuDebug(ArbolHistoria *arbol, Player *player, Nodo *nodoActual) {
    int opcion;
    int numPadres;
    PadreNodo *nodosPadres;
    do {
        printf("\n--- Menú de Depuración ---\n");
        printf("1. Buscar Nodo por ID\n");
        printf("2. Mostrar todos los nodos\n");
        printf("3. Verificar punteros nodos\n");
        printf("4. Max Estadísticas\n");
        // Aquí puedes añadir más opciones de depuración
        printf("0. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                debugNodo(); // Mostrar informacion de nodo especifico
                break;
            case 2:
                mostrarIdTodosNodos(); // Mostrar todas las ids guardadas en el conjunto
                break;
            case 3:
                nodosPadres = nodoActual->firstChild->parents;
                numPadres = nodoActual->firstChild->numParents;
                if(numPadres>1) printf("Nodo actual comparte hijos con %d nodo(s)\n", numPadres-1);
                else printf("Nodo actual es padre único de su hijo\n");
                break;
            case 4:
                maximizarEstadisticas(player); // Maximizar estadisticas y mostrarlas
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
          printf("3. Resistencia (Actual: %d/10)\n", player->endurance);
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
void mostrarProgresoHistoria(ArbolHistoria *arbol, Nodo *nodoActual) {
    // Calcular la altura total del árbol
    int alturaTotal = calcularAlturaArbol(arbol);


    printf("\n--- Progreso de la historia ---\n");

    if (alturaTotal > 0) {
        printf("\nAltura actual en la historia: %d de %d niveles\n", nodoActual->level, alturaTotal);

        // Calcular y mostrar el porcentaje de progreso
        float porcentajeProgreso = ((float) nodoActual->level / (float) alturaTotal) * 100;
        printf("Llevas: %.2f%% completo\n", porcentajeProgreso);
    } else {
        printf("No se puede calcular el progreso sin altura total válida.\n");
    }
}
Nodo* avanzarEvento(Nodo *nodoActual, Player *jugador, List *inv, Set *visitados) {
    if (nodoActual == NULL) {
        printf("No hay más eventos.\n");
        return NULL;
    }

    Nodo *nodoRaiz = buscarNodoPorId(1);
    Nodo *reinicio = buscarNodoPorId(102); // ID del nodo del minijuego tutorial

    // Revisar mapa para ver si nodo ya se visitó
    if ((map_search(visitados, &(nodoActual->id)) != NULL) && (nodoActual != nodoRaiz)) {
        printf("Ya has visitado este evento. No puedes volver a visitarlo.\n");
        return NULL;
    }
    // No marcar como visitado el nodo inicio y reinicio
    if (nodoActual != reinicio && nodoActual != nodoRaiz) {
        map_insert(visitados, &(nodoActual->id), nodoActual); // Marcar nodo como ya visitado
    }

    printf("\n\n%s\n", nodoActual->description); // Mostrar evento actual
    // Verificar si el nodo tiene un minijuego y llamarlo si es así
    if (nodoActual->isMinigame) {
        printf("¡Que comience la batalla!\n");
        iniciarBatalla(jugador, nodoActual->enemy, inv);
    }

    Nodo* hijoActual = nodoActual->firstChild; // Acceder al primer hijo
    if (hijoActual == NULL) { // Cuando ya no hay mas hijos
        printf("Fin del camino. No hay más eventos disponibles.\nRecuerda que esta es una demo, si quieres el juego completo, tendrás que suscribirte al Patreon por solo 20 UF a la semana! Gracias por jugar\n");
        return NULL;
    }

    Nodo* hijosValidos[10]; // Array para almacenar nodos hijos válidos
    int numHijosValidos = 0;

    while (hijoActual != NULL) { // Mostrar todas las opciones
        if (verificarCondiciones(hijoActual, jugador) || nodoActual == nodoRaiz) { // Verificar si la opción es válida
            // Verificar si el hijo ya fue visitado, excepto el tutorial
            if (map_search(visitados, &(hijoActual->id)) == NULL) {
                printf("%d. %s\n", numHijosValidos + 1, hijoActual->name); // Mostrar el nombre de la opción válida
                hijosValidos[numHijosValidos++] = hijoActual; // Agregar hijo a los hijos válidos
            } else {
                printf("X. %s (Ya visitado)\n", hijoActual->name); // Mostrar opción ya visitada
            }
        } else {
            printf("X. %s (No cumples con la habilidad)\n", hijoActual->name); // Mostrar opción inválida
        }

        Nodo* next = hijoActual->nextSibling;
        if (next == hijoActual) { // Detectar bucle infinito
            //printf("Bucle infinito detectado en el hijo: %s\n", hijoActual->name);
            break;
        }
        hijoActual = next;
    }

    if (numHijosValidos == 0) {
        printf("No hay opciones disponibles basadas en tus estadísticas o ya has visitado todas las opciones.\n");
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
    jugador.health = 80;
    jugador.endurance = 1;
    jugador.strenght = 1;
    jugador.charisma = 1;
    jugador.agility = 1;
    jugador.luck = 1;
    jugador.gold = 0;
    jugador.level = 1;
    List* inventario = list_create(); // inventario
    addItem(inventario, "Espada Básica", "Una espada simple y eficaz.", 1);
    addItem(inventario, "Poción de Curación", "Restaura 50 puntos de vida.", 3);
    
    //startStory(&jugador); // customizar personaje
    todosLosNodos = map_create(compararIds); // Inicializar conjunto de todos los nodos
    Set *visitados = map_create(compararIds); // Inicializar conjunto de todos los nodos visitados
    
    ArbolHistoria *arbol = (ArbolHistoria*)malloc(sizeof(ArbolHistoria));
    arbol->root = NULL;
 
    int idRaiz = 1;
    crearNodosHistoria(arbol, jugador, inventario); // Funcion para inicializar cada nodo y armar arbol
    Nodo *nodoActual = buscarNodoPorId(idRaiz); // Extraer raiz para empezar a recorrer
    
    int opcion;
    do {
        printf("\n--- Menú Principal ---\n");
        printf("1. Avanzar al Siguiente Evento\n");
        printf("2. Revisar Inventario\n");
        printf("3. Revisar Estadísticas\n");
        printf("4. Mostrar Progreso de la Historia (WIP)\n");
        printf("5. Menú Debug (DESARROLLADOR)\n");
        printf("6. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        
        if (jugador.health <= 0) {
            printf("\nHas perdido tu vida\nGame Over...\n");
            exit(1);
        } 
        switch (opcion) {
            case 1:
                nodoActual = avanzarEvento(nodoActual, &jugador, inventario, visitados);
                aplicarEfectos(nodoActual, &jugador, inventario);  // Aplicar efectos del nodo actual*/
                break;
            case 2:
                mostrarInventario(inventario);
                break;
            case 3:
                mostrarEstadisticas(&jugador, 1);
                break;
            case 4:
                //mostrarProgresoHistoria(arbol, nodoActual);
                break;
            case 5:
                menuDebug(arbol, &jugador, nodoActual);
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
