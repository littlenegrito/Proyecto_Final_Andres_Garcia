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
extern Map* todosLosNodos;
extern int numTotalNodos;

typedef struct Player{
    char *name;
    short strenght;       // Fuerza
    short agility;        // Agilidad
    short luck;           // Suerte
    short charisma;       // Carisma
    short endurance;      // Resistencia
    short health;         // Puntos Vida
    short gold;           // Dinero
    short level;

} Player;

typedef struct Enemy{
    char *name;
    short health;
    short strenght; 
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
    Player stats; // Tipo jugador
    int level;  // Profundidad
    int isMinigame; // Bool para saber si es un minijuego
    Enemy *enemy; // Tipo enemigo
    Condicion *condiciones; // Lista de condiciones para acceder a este nodo hijo
    struct Nodo *firstChild; // Primer hijo del nodo
    struct Nodo *nextSibling; // Hermano derecho del nodo
    struct PadreNodo* parents; // Lista de padres
    int numParents;
} Nodo;

typedef struct PadreNodo {
    struct Nodo *padre;
    struct PadreNodo *siguiente;
} PadreNodo;

typedef struct {
    Nodo *root;
} ArbolHistoria;

// Crear nodo evento y guardar en set
Nodo* crearEvento(int id, const char* nombre, const char* descripcion, Player stats, int level) {
    Nodo* nuevoNodo = (Nodo*)malloc(sizeof(Nodo));
    if (!nuevoNodo) {
        printf("Error al asignar memoria para un nuevo nodo.\n");
        exit(1);
    }
    nuevoNodo->id = id;
    nuevoNodo->name = strdup(nombre); // Usar strdup para asignar memoria y copiar la cadena
    nuevoNodo->description = strdup(descripcion); // Usar strdup para asignar memoria y copiar la cadena
    nuevoNodo->level = level;
    nuevoNodo->stats = stats;
    nuevoNodo->condiciones = NULL; // Lista de condiciones inicialmente vacía, luego se añaden
    nuevoNodo->isMinigame = 0; // No se marca minijuego
    nuevoNodo->enemy = NULL; // No contiene
    nuevoNodo->firstChild = NULL;
    nuevoNodo->nextSibling = NULL;
    nuevoNodo->parents = NULL;
    nuevoNodo->numParents = 0;
    if(map_search(todosLosNodos, &(nuevoNodo->id)) == NULL){ // Verificar que no esté en mapa
        map_insert(todosLosNodos, &(nuevoNodo->id), nuevoNodo); // Agregar id nodo al mapa global
        if(todosLosNodos == NULL){ // Verifica que se haya insertado el nodo en el mapa
            printf("Error al insertar nodo en el mapa.\n");
            exit(1);
        }
        numTotalNodos++; // Aumentar número de nodos
        return nuevoNodo;
    }
    else{ 
        printf("Este nodo ya existe en el mapa.\n");
        return NULL;
    }
}
// Crear nodo evento con minijuego y guardar en set
Nodo* crearEventoMinijuego(int id, const char* nombre, const char* descripcion, Player stats, Enemy *enemigo) {
    Nodo* nuevoNodo = (Nodo*)malloc(sizeof(Nodo));
    if (!nuevoNodo) {
        printf("Error al asignar memoria para un nuevo nodo.\n");
        exit(1);
    }
    nuevoNodo->id = id;
    nuevoNodo->name = strdup(nombre);
    nuevoNodo->description = strdup(descripcion);
    nuevoNodo->stats = stats;
    nuevoNodo->level = 0; 
    nuevoNodo->condiciones = NULL; // Lista de condiciones inicialmente vacía, luego se añaden
    nuevoNodo->isMinigame = 1; // Se marca minijuego
    nuevoNodo->enemy = enemigo; // Si contiene enemigo
    nuevoNodo->firstChild = NULL;
    nuevoNodo->nextSibling = NULL;
    nuevoNodo->parents = NULL;
    nuevoNodo->numParents = 0;
    
    if(map_search(todosLosNodos, &(nuevoNodo->id)) == NULL){ // Verificar que no esté en mapa
        map_insert(todosLosNodos, &(nuevoNodo->id), nuevoNodo); // Agregar id nodo al mapa global
        if(todosLosNodos == NULL){ // Verifica que se haya insertado el nodo en el mapa
            printf("Error al insertar nodo en el mapa.\n");
            exit(1);
        }
        numTotalNodos++; // Aumentar número de nodos
        return nuevoNodo;
    }
    else{ 
        printf("Este nodo ya existe en el mapa.\n");
        return NULL;
    }
}
// Agrega nodo hijo
void agregarHijo(Nodo* nodoPadre, Nodo* nodoHijo) {
    if (nodoPadre == NULL || nodoHijo == NULL) {
        printf("Error: El padre o el hijo no pueden ser nulos.\n");
        return;
    }

    // Ajustar padres
    PadreNodo *nuevoPadre = (PadreNodo*) malloc(sizeof(PadreNodo));
    nuevoPadre->padre = nodoPadre;
    nuevoPadre->siguiente = NULL;
    if (nodoHijo->parents == NULL) { // si nodo hijo no tiene padres
        nodoHijo->parents = nuevoPadre; // agregar padre al princpio
    }else{
    // Insertar el nuevo padre al final de la lista de padres del nodo hijo
        PadreNodo *ultimo = nodoHijo->parents;
        while (ultimo->siguiente != NULL) {
            ultimo = ultimo->siguiente; // siguiente padre
        }
        ultimo->siguiente = nuevoPadre; // agregar padre al final
    }
    nodoHijo->numParents++;
    
    // Ajustar hijos
    Nodo *ultimo = nodoPadre->firstChild;
    if (ultimo == NULL) { // si primer hijo no existe
        nodoPadre->firstChild = nodoHijo; // agregar al principio
    } else {
        while (ultimo->nextSibling != NULL) {
            ultimo = ultimo->nextSibling; // siguiente hermano
        }
        ultimo->nextSibling = nodoHijo; // agregar hijo al final
    }
 
}

// Funcion de busqueda en arbol por id
Nodo* buscarNodoPorId(int id) {
    MapPair *pair = map_search(todosLosNodos, &id);
    if (pair != NULL) {
        return (Nodo*)(pair->value); // Devuelve el nodo asociado a la clave encontrada
    }
    return NULL; // Si no se encuentra, devuelve NULL
}
// Funcion para verificar si dos nodos conectan al mismo hijo
int compararHijos(Nodo* nodo1, Nodo* nodo2) {
    Nodo* hijo1 = nodo1->firstChild;
    Nodo* hijo2 = nodo2->firstChild;

    while (hijo1 != NULL && hijo2 != NULL) {
        if (hijo1->id != hijo2->id) return 0;
        hijo1 = hijo1->nextSibling;
        hijo2 = hijo2->nextSibling;
    }

    return (hijo1 == NULL && hijo2 == NULL);
}
// Funcion ajustar punteros de padres y dejar solo 1
void consolidarNodos(Nodo *nodoPrincipal) {
    Nodo *nodoHijo = nodoPrincipal->firstChild; // Acceder al primer nodo hijo
    while (nodoHijo != NULL) {
        PadreNodo *padreActual = nodoHijo->parents; // Acceder al primer padre
        PadreNodo *padrePrevio = NULL; // Guardar padre previo
        // Recorrer la lista de padres del nodo hijo
        while (padreActual != NULL) {
            if (padreActual->padre != nodoPrincipal) { // Si el padre actual no es el principal
                if (padrePrevio == NULL) {
                    nodoHijo->parents = padreActual->siguiente; // Eliminar padre actual
                } else {
                    padrePrevio->siguiente = padreActual->siguiente; // Saltarse padre actual
                }
                // Liberar la memoria del nodo de la lista de padres
                PadreNodo *temp = padreActual; 
                padreActual = padreActual->siguiente;
                free(temp);
            } else { // Si el padre actual es el principal
                padrePrevio = padreActual; // Guardar actual
                padreActual = padreActual->siguiente;
            }
        }

        // Asegurar que el nodo principal sea el único padre del nodo hijo
        if (nodoHijo->parents == NULL) { // Si no quedaron padres
            PadreNodo *nuevoPadre = (PadreNodo*)malloc(sizeof(PadreNodo)); // Crear padre unico
            nuevoPadre->padre = nodoPrincipal;
            nuevoPadre->siguiente = NULL; // No hay siguiente padre
            nodoHijo->parents= nuevoPadre; // Guardar padre unico
        } 
        nodoHijo = nodoHijo->nextSibling; // Avanzar al siguiente nodo hijo
    }
}
// Liberar memoria de un nodo y sus hijos
void liberarNodo(Nodo* nodo) {
    if (nodo == NULL) return;

    liberarNodo(nodo->firstChild);
    liberarNodo(nodo->nextSibling);

    free(nodo->name);
    free(nodo->description);

    Condicion *cond = nodo->condiciones;
    while (cond != NULL) {
        Condicion *temp = cond;
        cond = cond->next;
        free(temp->tipo);
        free(temp);
    }

    free(nodo);
}
// Liberar memoria de un árbol
void liberarArbol(ArbolHistoria* arbol) {
    if (arbol == NULL) return;
    liberarNodo(arbol->root);
    free(arbol);
}
// Crear tipo condicion e inicializar
Condicion* crearCondicion(char *tipo, int valor) {
    Condicion *nueva = (Condicion*)malloc(sizeof(Condicion));
    nueva->tipo = strdup(tipo);
    nueva->valor = valor;
    nueva->next = NULL;
    return nueva;
}
// Agregar condicion personalizada a un nodo
void agregarCondicion(Nodo *hijo, char *tipo, int valor) {
    Condicion *nueva = crearCondicion(tipo, valor);
    if (hijo->condiciones == NULL) {
        hijo->condiciones = nueva;
    } else {
        Condicion *actual = hijo->condiciones;
        while (actual->next != NULL) {
            actual = actual->next;
        }
        actual->next = nueva;
    }
}
// Verificar condiciones para acceder a un nodo hijo
int verificarCondiciones(Nodo *hijo, Player *jugador) {
    Condicion *condicion = hijo->condiciones;
    while (condicion != NULL) {
        if (strcmp(condicion->tipo, "charisma") == 0 && jugador->charisma < condicion->valor) {
            return 0;
        } else if (strcmp(condicion->tipo, "strenght") == 0 && jugador->strenght < condicion->valor) {
            return 0;
        } else if (strcmp(condicion->tipo, "agility") == 0 && jugador->agility < condicion->valor) {
                return 0;
        } else if (strcmp(condicion->tipo, "endurance") == 0 && jugador->endurance < condicion->valor) {
                return 0;
        } else if (strcmp(condicion->tipo, "luck") == 0 && jugador->luck < condicion->valor) {
                return 0;
        } else if (strcmp(condicion->tipo, "gold") == 0 && jugador->strenght < condicion->valor) {
                return 0;
        } else if (strcmp(condicion->tipo, "level") == 0 && jugador->strenght < condicion->valor) {
                return 0;
        } else if (strcmp(condicion->tipo, "gold") == 0 && jugador->gold < condicion->valor){
                return 0;
        }
        condicion = condicion->next;
    }
    return 1;
}
// Función para aplicar los efectos del nodo en el jugador
void aplicarEfectos(Nodo *nodo, Player *jugador, List *inventario) {
    // Aplicar cambios en las estadísticas del jugador
    if(jugador != NULL){
    jugador->strenght += nodo->stats.strenght;
    jugador->agility += nodo->stats.agility;
    jugador->luck += nodo->stats.luck;
    jugador->charisma += nodo->stats.charisma;
    jugador->endurance += nodo->stats.endurance;
    jugador->health += nodo->stats.health;
    if(nodo->stats.health > jugador->health) jugador->health = 0;
    jugador->gold += nodo->stats.gold;
    jugador->level += nodo->stats.level;
    }

    // Aquí puedes agregar lógica para manipular el inventario según el nodo

}

Enemy* crearEnemigo(const char* name, short health, short strenght){
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

void displayHealth(Player *player, Enemy *enemy) {
  printf("\nVida %s: %d | Vida %s: %d\n", player->name, player->health, enemy->name, enemy->health);
}
void showOptions() {
  printf("\nOpciones:\n");
  printf("1. Atacar\n");
  printf("2. Usar Ítem\n");
  printf("3. Huir\n");
  printf("Elija una opción: ");
}

void endCombat(Stack* actionStack) {
    printf("\nEl combate ha terminado.\n");
    // Vaciar la pila de acciones
    while (stack_top(actionStack) != NULL) {
        stack_pop(actionStack); // Limpiar la pila
    }
}

void announceWinner(unsigned short playerHealth, unsigned short enemyHealth) {
  if (enemyHealth <= 0) {
      printf("\n¡Has ganado!\n\n");
  } else if (playerHealth <= 0) {
      printf("\nHas muerto.\n\n");
  }
}

void useItem(Player *player, List *inventario) {
  int heal = 50;
  mostrarInventario(inventario);
  printf("\nHas usado un ítem y recuperado %d puntos de vida.\n", heal);
  player->health += heal;
}

void attack(Player *player, Enemy *enemy) {
    int baseDamage = rand() % 15 + 1; // Daño base entre 1 y 15
    int totalDamage = baseDamage * player->strenght; // Multiplicar por la fuerza del jugador

    // Asegurarse de que el daño total sea al menos 1
    totalDamage = (totalDamage < 1) ? 1 : totalDamage;

    printf("\n--> Has atacado al enemigo y causado %d puntos de daño.\n", totalDamage);

    if (totalDamage >= enemy->health) {
        enemy->health = 0; // Asegurarse de que la vida no sea negativa
    } else {
        enemy->health -= totalDamage;
    }
}

void ataqueEnemigo(Player *player, Enemy *enemy) {
    // El enemigo ataca al jugador si el jugador sigue vivo
    int baseDamage = rand() % 15 + (player->strenght); // Daño base del enemigo ajustado por fuerza jugador
    baseDamage = baseDamage * enemy->strenght; // Daño total inicial antes de aplicar resistencia
    int totalDamage = baseDamage; // Daño total inicial;

    // Calcular reducción de daño base por resistencia
    int resistance = player->endurance * baseDamage / 11; // 9% del daño total por punto de resistencia
    resistance = (resistance > 10) ? 10 : resistance; // Límite máximo de absorción
    totalDamage -= resistance;

    // Asegurarse de que el daño total sea al menos 1
    totalDamage = (totalDamage < 1) ? 1 : totalDamage;

    printf("\n--> El enemigo te ha atacado y causado %d puntos de daño", baseDamage);
    if(resistance > 0) printf(", de los cuáles absorbiste %d puntos", resistance);
    printf(".\n\nDaño Total : %d\n", totalDamage);

    if (totalDamage >= player->health) {
        player->health = 0; // Asegurarse de que la vida no sea negativa
    } else {
        player->health -= totalDamage;
    }
}

void iniciarBatalla(Player *jugador, Enemy *enemigo, List *inventario) {
    Stack* actionStack = stack_create();
    stack_push(actionStack, "player");
    
    while (jugador->health > 0 && enemigo->health > 0 && stack_top(actionStack) != NULL) {
        displayHealth(jugador, enemigo);

        char* currentTurn = stack_top(actionStack);
        if (strcmp(currentTurn, "player") == 0) { // Turno Jugador
            showOptions();
            int option;
            scanf("%d", &option);

            switch (option) {
                case 1:
                    attack(jugador, enemigo);
                    if (enemigo->health <= 0) endCombat(actionStack);
                    break;
                case 2:
                    useItem(jugador, inventario);
                    break;
                case 3:
                    endCombat(actionStack);
                    break;
                default:
                    printf("\nOpción inválida.\n");
                    continue;
            }
            if (enemigo->health > 0) { // Solo realiza el turno del enemigo si el enemigo sigue vivo
                    stack_push(actionStack, "enemy");
                }
            
        } else if (strcmp(currentTurn, "enemy") == 0) { // Turno Enemigo
            printf("\nTurno del enemigo.\n");
            ataqueEnemigo(jugador, enemigo);
            // Si el jugador sigue vivo, es su turno
            if (jugador->health > 0) {
                stack_push(actionStack, "player");
            }else endCombat(actionStack);
        }
                
    }
    announceWinner(jugador->health, enemigo->health);
    stack_clean(actionStack);
}

void crearNodosHistoria(ArbolHistoria *arbol, Player jugador, List *inventario) {
    Player statsDefault = {0};
    Player statsDead = {0};
    statsDead.health -= 9000;
    
    Condicion *condicionFuerza1 = crearCondicion("strenght", 3);
    Condicion *condicionResistencia1 = crearCondicion("endurance", 3);
    Condicion *condicionAgilidad1 = crearCondicion("agility", 3);
    Condicion *condicionCarisma1 = crearCondicion("charisma", 3);
    Condicion *condicionSuerte1 = crearCondicion("luck", 3);
    
    Condicion *condicionFuerza2 = crearCondicion("strenght", 5);
    Condicion *condicionResistencia2 = crearCondicion("endurance", 5);
    Condicion *condicionAgilidad2 = crearCondicion("agility", 5);
    Condicion *condicionCarisma2 = crearCondicion("charisma", 5);
    Condicion *condicionSuerte2 = crearCondicion("luck", 5);

    Condicion *condicionDinero1 = crearCondicion("gold", 3);
    Condicion *condicionDinero2 = crearCondicion("gold", 5);
    Condicion *condicionDinero3 = crearCondicion("gold", 10);
    Condicion *condicionDinero4 = crearCondicion("gold", 15);
    Condicion *condicionDinero5 = crearCondicion("gold", 20);
    Condicion *condicionDinero6 = crearCondicion("gold", 25);
    Condicion *condicionDinero7 = crearCondicion("gold", 50);

    Enemy* enemigoOgro = crearEnemigo("Ogro", 80, 3);
    Enemy* enemigoOgro2 = crearEnemigo("Ogro", 50, 2);
    Enemy* enemigoVendedor = crearEnemigo("Vendedor", 70, 1); 
    Enemy* enemigoVendedor2 = crearEnemigo("Vendedor Moribundo", 5, 1); 
    Enemy* enemigoLeñador = crearEnemigo("Leñador", 80, 3); 
    Enemy* enemigoLadron = crearEnemigo("Ladron", 75, 1);
    Enemy* enemigoGoblin = crearEnemigo("Goblin", 55, 2);
    Enemy* enemigoGoblin2 = crearEnemigo("Goblin", 75, 3);
    Enemy* enemigoThorne = crearEnemigo("Guerrero Thorne", 120, 3);

    
    // Nodo raíz
    arbol->root = crearEvento(1, "Inicio", "Te despiertas en un bosque oscuro completamente solo. Estás sangrando de la pierna, el malestar del hambre y la sed te drena la energía cada segundo, pero aún tienes suficiente fuerza para caminar hacia la luz.\nTe encuentras con dos caminos, uno a la izquierda y otro a la derecha.\n", jugador, 0);

    // Nodo 1A: Ir a la izquierda
    Player statsLeft = {0};
    statsLeft.health += 5;
    Nodo *startLeft = crearEvento(2, "Ir a la izquierda", "Llegas a un río y tomas agua. Ganas 5 puntos de salud. Miras alrededor y encuentras una cueva en la distancia, pero el río sigue hacia el este y parece que conecta a un cuerpo de agua más grande.\n", statsLeft, 1);
    agregarHijo(arbol->root, startLeft);

        // Nodo 2A.A: Seguir el río
        Nodo *startLeft_1 = crearEvento(3, "Seguir el río y buscar un camino", "Sigues el río pero antes de llegar a la esquina de una roca, escuchas un gruñido de atrás. Te das la vuelta y ves a un ogro acercándose. Aún no te ha visto y tienes tiempo para escapar o esconderte.\n", statsDefault, 2);
        agregarHijo(startLeft, startLeft_1);

            // Nodo 3A.A.A: Correr del ogro
            Nodo *ogreRun = crearEvento(4, "Correr por tu vida", "Decides correr antes de que el ogro se dé cuenta de tu presencia, pero en un momento de confusión, te desorientas y olvidas de dónde viniste. No sabes lo que está esperando en la esquina de la roca, pero tienes un camino directo de regreso al bosque.\n", statsDefault, 3);
            agregarHijo(startLeft_1, ogreRun);

                // Nodo 4A.A.A.A: Correr recto
                Nodo *runStraight = crearEvento(5, "Seguir corriendo recto", "Sigues corriendo recto, siguiendo el río, pero al girar la esquina de las rocas, te topas con un vendedor distraído. Chocas con él pero mantienes tu balance, aunque él desafortunadamente cae al suelo y ahora se convierte en la presa del ogro. Instintivamente, tratas de seguir corriendo, pero al mirar atrás, te das cuenta que el vendedor tiene suministros útiles\n", statsDefault, 4);
                agregarHijo(ogreRun, runStraight);

                    // Nodo 5A.A.A.A.A: Defender Vendedor (BATALLA)
                    Nodo *defendTrader = crearEventoMinijuego(6, "Defender al vendedor", "Antes de que el ogro devore al vendedor, agarras una espada que este botó y decides defenderlo, poniéndote en frente del vendedor para absorber el primer golpe del ogro con la espada. El vendedor actúa rápido y le arroja una poción de daño que deja al ogro aturdido, y aprovechas a seguir atacando\n", statsDefault, enemigoOgro);
                    agregarHijo(runStraight, defendTrader);

                    // Nodo 5A.A.A.A.B: Dejar al vendedor
                    Nodo *sacrificeTrader = crearEvento(7, "Dejar al vendedor", "A pesar de tener suministros a su disposición, arriesgar tu vida para salvar al vendedor no lo vale, especialmente al enfrentar un monstruo tan terrible, así que sigues corriendo. Intentas no mirar atrás mientras que el ogro se devora al vendedor, y apenas llegas a una bifurcación en un valle, evalúas los dos caminos a tus lados, uno siguiendo el río y otro hacia las colinas del valles. Como los dos están vacíos, no piensas mucho antes de ir en una dirección.\n", statsDefault, 5);
                    agregarHijo(runStraight, sacrificeTrader);

                        // Nodo 6A.A.A.A.B.A: Ir hacia las colinas
                        Nodo *interseccionA = crearEvento(47, "Ir hacia las colinas", "Lentamente, estando alerta en caso de que el ogro te haya seguido, bordeas el bosque mientras vas por el sendero de tierra atravesando el valle cuando de repente esuchas el susurro de los arbustos detrás tuyo. Te das la vuelta a revisar, pero la densidad del bosque crea una oscuridad que tu visión no puede atravesar, por lo que te acercas un poco.\n\nDe la nada, tres individuos saltan de los arbustos y te sorprenden, rodeandote y apuntando sus armas de inmediato. Al principio, adoptan la defensas, pero después de ver que estás desarmado, retroceden un poco y te dan espacio para recuperar tu aliento y calmarte.\n\nEl grupo consiste de una hechicera por el sombrero y el bastón mágico que tiene en la mano, un guerrero de pelo corto y marrón con un hacha más grande que su cuerpo entero, y una arquera rubia con orejas largas, por lo que la identificas como una elfa. Solo la hechicera se acerca para ofrecer ayuda después de ver tu condición, pero antes de pasarte unas vendas y agua, te mira directo a los ojos y pregunta\n-Hey tú. Quién eres y qué haces aquí? Estás en nuestro territorio-\n", statsDefault, 6);
                        agregarHijo(sacrificeTrader, interseccionA);

                        // Nodo 6A.A.A.A.B.A: Ir por el rio
                        Nodo *interseccionB = crearEvento(48, "Ir por el rio", "El sendero al valle está muy descubierto, por lo que prefieres quedarte cerca del río y seguir el camino más estrecho y oscuro. Después de un tramo desolado, el río conecta a una laguna adelante. No logras ver qué hay detrás de unos árboles rodeando el cuerpo de agua, así que te acercas a la laguna. Asomándote detrás de un árbol, descubres una cabaña con una pequeña granja de pollos en el patio, pero notas que las luces están prendidas, así que te tomas un segundo para pensar en qué hacer.\n", statsDefault, 6);
                        agregarHijo(sacrificeTrader, interseccionB);
    
                // Nodo 4A.A.A.B: Regresar al bosque (END 3)
                Nodo *backForest= crearEvento(8, "Doblar y regresar al bosque", "Para despistar al ogro, decides dar un giro brusco y regresar al bosque. El monstruo te sigue pero logras correr más rápido, aunque en el intento de escapar, no prestas atención a tus pasos y te tropiezas con un tronco justo antes de caer bajo una colina.\n", statsDefault, 4);
                agregarHijo(ogreRun, backForest);
    
                        // Nodo 5A.A.A.B.A: Aguantas caida (END 3)
                        Player statsForest = {0};
                        statsForest.health -= 10; 
                        
                        Nodo *backForestRes = crearEvento(61, "Aguantas la caida (END 3)", "La caída no es tan severa, pero al llegar al suelo, te pegas en la cabeza con una roca y te quita 10 de vida, pero eres capaz de seguir corriendo directo hasta que llegas a lo que parece ser un campamento.\n", statsForest, 5);
                        backForestRes->condiciones = condicionResistencia1;
                        agregarHijo(backForest, backForestRes);

                        // Nodo 5A.A.A.B.B: Caes inconsciente
                        Player statsForestFall = {0};
                        statsForestFall.health -= 25; 
                        Nodo *backForestNight = crearEvento(62, "La caida te deja incapacitado", " La caída no es tan grande, pero al llegar al suelo, te pegas en la cabeza con una roca y quedas inconsciente hasta la noche. Has perdido 25 puntos de vida y estás en la peor condición posible, ni sientes la cabeza o tienes la capcidad de moverte.\n", statsForestFall, 5);
                        agregarHijo(backForest, backForestNight);
    
            // Nodo 3A.A.B: Esconderse del ogro
            Nodo *ogreHide = crearEvento(9, "Esconderse en la cueva", "Para evitar el campo de visión del ogro, decides correr dentro de la cueva. El ogro es lento y le toma unos segundos antes de llegar a la entrada de la cueva. En pánico, te refugias en la oscuridad, manteniéndote en el suelo hasta que sientes el mango de una espada. Equipas la espada para defenderte pero esperas a que el ogro pase. Por suerte, no investigó la cueva, y luego de un rato, asomas la cabeza. El ogro ya no está, pero ves que en la posición que te encontrabas anteriormente, justo antes de la esquina de las rocas, el monstruo dejó un rastro de sangre que se dirige al bosque.\n", statsDefault, 3);
            agregarHijo(startLeft_1, ogreHide);
    
                // Nodo 4A.A.B.A: Seguir rastro de sangre
                Nodo *followBlood = crearEvento(10, "Seguir rastro de sangre", "Sigues el rastro de sangre dejado por el ogro hasta llegar a un claro en el bosque, donde encuentras a un hombre desangrandose en el centro. Inmediatamente, te pide ayuda y señala adelante, donde continua el rastro de sangre por el bosque, yendo aún más profundo a la oscuridad.\n\n-Se fue por allá? Estás seguro?- Preguntas antes de acercarte en caso de que el ogro se encuentre cerca. El hombre asiente con la cabeza pero cada vez se mueve menos por la pérdidad de sangre.\n\n En pánico, miras a tu alrededor antes de ayudar al hombre, pero como no tienes nada a tu disposición, intentas buscar la mochila del herido. Parece que el ogro rajó la mochila y se llevo la mitad, por lo que encuentras comida y oro, pero nada de suministros para ayudar al hombre. En ese momento, este agarra tu camisa con sus manos sangrientas y apunta de nuevo hacia adelante, guiandote hacia el monstruo\n", statsDefault, 4);
                agregarHijo(ogreHide, followBlood);

                    // Nodo 5A.A.B.A.A: Ayudar al vendedor
                    Nodo *helpVendor = crearEvento(23, "Seguir al ogro en busca de suministros", "Decides seguir el rastro del ogro con la esperanza de encontrar suministros que puedan salvar al hombre. Avanzas por el bosque, siguiendo las marcas de sangre y las ramas rotas. Finalmente, llegas a un nido de arbustos y arboles, donde te asomas y ves al ogro hurgando en un saco de provisiones.\n", statsDefault, 5);
                    agregarHijo(followBlood, helpVendor);
    
                    // Nodo 5A.A.B.A.B: Saquear vendedor
                    Player statsLoot = {0};
                    statsLoot.health += 10; 
                    statsLoot.gold += 100; 
                    Nodo *lootVendor = crearEvento(24, "Saquear el cuerpo y dejarlo morir", "No piensas arriesgar tu vida para salvar otra, así que aprovechas a robar todo lo que encuentras en la mochila mientras que el hombre solo te mira y te condena en silencion. Encuentras dos panes que no puedes resistir y empiezas a comer enfrente del hombre. Aún sigue vivo, pero después de ser testigo de tu crueldad, cierra los ojos y acepta su destino. Tomas esto como una señal para irte y , pero no sin arte robar todas las monedas que puedas encontrar en la mochila. Mientras te alejas del cuerpo del hombre, sientes una extraña mezcla de satisfacción y culpa y escuchas pasos por detrás, directamente desde donde viniste. Ganaste 10 puntos de vida y 100 de oro\n", statsLoot, 5);
                    agregarHijo(followBlood, lootVendor);
    
                        // Nodo 6A.A.B.A.B.A: Seguir ogro
                        Nodo *followOgreTrail = crearEvento(25, "Ignorar el ruido y seguir el rastro del ogro", 
                            "Decides ignorar el ruido y seguir el rastro del ogro, esperando encontrar más recursos o al propio ogro para enfrentarlo. Avanzas por el bosque, cada vez más oscuro y denso, hasta que escuchas unos gruñidos cercanos. Te das cuenta de que estás muy cerca del ogro, pero encuentras un nido de arbustos y arboles, con la otra mitad de la mochila esparcida en el suelo.\n", statsDefault, 6);
                        agregarHijo(lootVendor, followOgreTrail);
    
                            // Nodo 7A.A.B.A.B.A.A: Agarrar cosas (AGI 3)
                            Nodo *grabLoot = crearEvento(27, "Tratar de agarrar suministros (AGI 3)", 
                                "\n", statsDefault, 7);
                            grabLoot->condiciones = condicionAgilidad1;
                            agregarHijo(followOgreTrail, grabLoot);
                            agregarHijo(helpVendor, grabLoot);

                            // Nodo 7A.A.B.A.B.A.b: Pelear ogro (BATALLA)
                            Nodo *fightOgre = crearEventoMinijuego(53, "Luchar contra el ogro", 
                                "Te preparas para atacar al ogro. Observas sus movimientos y esperas el momento oportuno. El ogro esta guardando los suministros, así que no hay de otra y pegas un salto para pillarlo por sorpresa. Finalmente, decides que es hora de actuar.\n", statsDefault, enemigoOgro);
                            agregarHijo(followOgreTrail, fightOgre);
                            agregarHijo(helpVendor, fightOgre);
    
                        // Nodo 6A.A.B.A.B.B: Explorar sonido
                        Nodo *exploreNoise = crearEvento(26, "Investigar el ruido en el bosque", 
        "Te acercas cautelosamente al origen del ruido. Entre los arbustos, descubres a un grupo de ladrones que parecen haberte visto robar al hombre. Uno de ellos se adelanta y te dice -Parece que has tenido un buen botín. ¿Por qué no compartes un poco con nosotros?-\nTe le ríes y tratas de ignorarlos, pero luego te das cuenta de que no están bromeado y se acercan a intimidarte\n", statsDefault, 6);
                        agregarHijo(lootVendor, exploreNoise);

                            // Nodo 7A.A.B.A.B.B.A: Luchar contra los ladrones
                            Nodo* fightThieves = crearEventoMinijuego(77, "Enfrentarte a los ladrones", 
                                "Decides que no vas a dejar que los ladrones te intimiden. Te preparas para la batalla, agarrando una daga que traia el vendedor y blandiendola con determinación. Los ladrones se ríen y sacan sus propias armas, listos para pelear, pero como están confiados, solo se te acerca el más grande para intimidarte mientras te pide que pases el oro. No piensas en ceder y tratas de apuñalarlo, pero este esquiva y con una sonrisa macabra, se lanza hacia ti.\n", 
                                statsDefault, enemigoLadron);
                            agregarHijo(exploreNoise, fightThieves);

                            // Nodo 7A.A.B.A.B.B.B: Mentir a ladrones (CH5)
                            Nodo* negotiateThieves = crearEvento(78, "Engañar a ladrones (CH 5)", 
                                "En lugar de pelear, decides intentar negociar con los ladrones. Con una voz firme y segura, dices -Escuchen, sé lo que buscan, pero me temo a que no tenía nada mas que pan. Hay más en este bosque que solo el botín de este hombre, y algo me dice de que el monstruo responsable de esto se dirigio para allá- Señalas el rastro adelante, llamando la atención de los ladrones, los cuales se toman un tiempo en pensar y tratan de descifrar si les estás mintiendo.\nPero antes de que digan algo, empiezas a dirigirte al rastro que lleva al ogro antes de darte la vuelta e insistir -Vamos, acaso no querían oro? ¿Por qué no trabajamos juntos y encontramos algo más valioso?-\nTu actitud logra convencer a los ladrones, y juntos embarcan en una misión para encontrar al ogro que mató al vendedor\n", 
                                statsDefault, 7);
                            negotiateThieves->condiciones = condicionCarisma2;
                            agregarHijo(exploreNoise, negotiateThieves);

                            // Nodo 7A.A.B.A.B.B.C: Correr hacia el ogro
                            Nodo* runToOgre = crearEvento(79, "Correr en dirección contraria", 
                                "Decides que la mejor opción es correr hacia una zona más oscura y estrecha del bosque. Los ladrones te persiguen sin pensarlo así que decides saltar hacia un nido de arbustos y árboles, el cuál parece ser seguro al principio. Pero al esconderte, te encuentras directamente al lado del ogro y el resto de la mochila del vendedor con los suministros esparcidos en el suelo. No te das cuenta hasta que ya es demasiado tarde y estás cara a cara con la bestia.\n", 
                                statsDefault, 7);
                            agregarHijo(exploreNoise, runToOgre);
                        
                                // Nodo 8A.A.B.A.B.B.C.A: Defenderte del ogro
                                Player statsFaceOgre = {0};
                                statsFaceOgre.health -= 10;
                                Nodo* faceOgre = crearEventoMinijuego(80, "Defenderte contra el ogro", 
                                    "El ogro te mira con ojos llenos de odio. Sabes que no hay manera de escapar, así que te preparas para la pelea de tu vida, tratando de defenderte de su golpe inicial, el cuál te manda volando hacia los ladrones. Pierdes 10 puntos de vida. Por un momento, estos estaban preparados para darte una paliza, pero apenas ven al ogro persiguiendote, estos empiezan a gritar y correr. No hay suficiente espacio para todos ustedes, y solo un ladron alcanza a escapar, mientras que los otros dos están forzados a enfrentarse al ogro\n\nA pesar de ser dos y estar armados, los ladrones solo pudieron delibitar al ogro con cortes superficiales antes de perecer ante las garras del monstru. Mientras aún estás en el suelo, ves como el ogro elimina a los dos ladrones como si fueran moscas, pero antes de que te ataque de nuevo, agarras una de las espadas de los ladrones muertos, te levantes, y aprovechas a atacar mientras el ogro está desgastado y aturdido.\n", statsFaceOgre, enemigoOgro2);
                                agregarHijo(runToOgre, faceOgre);
    
                                // Nodo 8A.A.B.A.B.B.C.B: Escapar ogro
                                Player statsEscapeOgre = {0};
                                statsEscapeOgre.gold += 250;
                                statsEscapeOgre.luck += 1;
                                Nodo* escapeOgre = crearEvento(81, "Escapar ogro (AGI 5)", "Eres lo suficientemente rápido para reaccionar antes de que el ogro te ataque. Usando toda tu agilidad, te lanzas hacia un lado, esquivando sus golpes y logrando poner algo de distancia entre ustedes.\nCon movimientos ágiles y rápidos, diriges al ogro hacia los ladrones. Confundido y enfurecido, el ogro se vuelve contra ellos, comenzando a atacarlos en lugar de a ti.\n\nAprovechando el caos, te deslizas alrededor del campo de batalla, recogiendo rápidamente los suministros del vendedor caído. Con los gritos y sonidos de la pelea a tus espaldas, logras escabullirte en el bosque, llevándote más suministros contigo, incluyendo una bolsa de oro y un amuleto de la suerte que te bendice con fortuna, a diferencia de los pobres ladrones que no pueden hacer nada excepto gritar hasta que el ogro termine con sus miserables vidas. Ganaste 250 de oro y un punto de suerte\n", statsEscapeOgre, 8);
                                escapeOgre->condiciones = condicionAgilidad2;
                                agregarHijo(runToOgre, escapeOgre);

    
                // Nodo 4A.A.B.B: Seguir camino hasta llegar a bifurcación
                Nodo *crossroads = crearEvento(11, "Seguir el camino por el río", "Cualquier cosa que haya pasado con el ogro, seguramente es peligroso, así que evitas el peligro al seguir recto. El río te lleva a un valle con un camino de tierra que parece ser de una ruta transitada. Tienes muchas direcciones para ir, pero como no ves mucho más que colinas en frente, revisas tu izquierda y derecha. A la izquierda, el camino con el río sigue a una zona más oculta pero que parece estar vacía. En cambio, a la derecha, ves dos personas corriendo por sus vidas. Por la distancia, no logras reconocer que les está pasando, pero después de unos segundos, ves una horda de duendes persiguiéndolos. Los dos individuos en pánico se están acercando a tu posición, pero es poco probable que no sobrevivan ya que los duendes están a punto de acorralarlos.\n", statsDefault, 4);
                agregarHijo(ogreHide, crossroads);
    
                    // Nodo 5A.A.B.B.A: Defender contra duendes (BATALLA)
                    Nodo *interveneAttack = crearEventoMinijuego(12, "Ayudar a defender contra duendes", "Como tienes una espada y aún no has sido visto por los duendes, decides ayudar un poco, siendo cauteloso para atacar a los duendes más aislados y distraer a la horda.\n", statsDefault, enemigoGoblin);
                    agregarHijo(crossroads, interveneAttack);

                    Nodo* ayudarGrupo = crearEvento(49, "Continuar", "Al matar al duende, el resto de la horda se esparce un poco, dejando a que los dos individuos retrocedan contigo y se reagrupen para tomar pociones y recuperarse. En ese momento, alcanzas a verlos de cerca, una parece ser una hechicera por el sombrero y el bastón mágico que tiene en la mano, y el otro es guerrero de pelo corto y marrón con un hacha más grande que su cuerpo entero. La hechicera brevemente te mira con gratitud antes de regresar a la pelea, con el guerrero corriendo directo al combate para atraer a los duendes mientras que la chica forma un hechizo. No sabes que hacer aparte de mirar, y aunque sigues con energía para luchar, la hechicera hace un gesto para que retrocedas.\n\nEl guerrero parece estar lanzandose a los duendes, pero justo antes de que lo agarren, este se tira al suelo y se cubre la cabeza, ya que la hechicera apunta su bastón y lanza un rayo tan brillante que aturde a los duendes por suficiente tiempo para que la hechicera carque su ataque y utilice toda su magia para crear una bola de luz y electricidad que asusta a los duendes.\n\nAsombrado pero temblando al ser testigo de tal poder, te quedas mirando a la hechicera mientras que esta corre hacia su compañero, ayudandolo a pararse después de que los duendes hayan escapado. Sin embargo, los dos se ponen a perseguirlos hacia la colina adelante, por lo que sigues detrás de ellos hasta que se detienen en la cima. Los duendes ya se han ido por el sendero hacia un conjunto de montañas a la distancia, y con el guerrero y la hechicera sin aliento, tratas de introducirte y encontrar una explicación de lo que acabas de ver.\n", statsDefault, 3);
                    agregarHijo(interveneAttack, ayudarGrupo);
            
                    // Nodo 5A.A.B.B.B: Ir al otro lado
                    Nodo *avoidConflict = crearEvento(13, "Evitar conflicto yendo a la otra dirección", "No quieres ser parte de la pelea, así que te das la vuelta y vas en el sentido contrario lo antes posible, tratando de no mirar atrás para no arriesgar ser visto. Puedes escuchar los individuos gritando por ayuda mientras son masacrados por los duendes, pero esto solo te motiva a correr más rápido hasta llegar a una distancia segura.\n\nCuando ya no puedes escuchar la pelea, te das la vuelta a revisar tu alrededor y ves que el río conecta a una laguna adelante. No logras ver qué hay detrás de unos árboles rodeando el cuerpo de agua, así que te acercas a la laguna. Asomándote detrás de un árbol, descubres una cabaña con una pequeña granja de pollos en el patio, pero notas que las luces están prendidas, así que te tomas un segundo para pensar en qué hacer.\n", statsDefault, 5);
                    agregarHijo(crossroads, avoidConflict);
    
                        // Nodo 6A.A.B.B.B: Tocar la puerta
                        Nodo *tocarPuerta = crearEvento(28, "Tocar la puerta y presentarte", "Tocas la puerta\n", statsDefault, 6);
                        agregarHijo(avoidConflict, tocarPuerta);
                        agregarHijo(interseccionB, tocarPuerta);
            
                        // Nodo 6A.A.B.B.B: Robar gallina
                        Nodo *robarGallina = crearEvento(29, "Robar una gallina para comer", "Te acercas sigilosamente a la granja y logras atrapar una gallina sin que nadie te vea al principio. Sin embargo, justo cuando estás a punto de irte, escuchas pasos apresurados detrás de ti. La puerta trasera de la granja se abre, y de ahí sale un leñador llevando comida a las gallinas, pero a penas te ve, grita enfurecido y saca su hacha para confrontarte.\n", statsDefault, 6);
                        agregarHijo(avoidConflict, robarGallina);
                        agregarHijo(interseccionB, robarGallina);

                            // Nodo 7A.A.B.B.B.A: Enfrentar al leñador
                            Nodo *enfrentarLenador = crearEventoMinijuego(66, "Enfrentar al leñador", "No te queda otra opción que enfrentarte al leñador. Preparas tu arma y te dispones a luchar. El leñador, armado con su hacha, no parece dispuesto a dejarte ir sin una pelea.\n", statsDefault, enemigoLeñador);
                            agregarHijo(robarGallina, enfrentarLenador);
                        
                            // Nodo 7A.A.B.B.B.B: Correr (AGI 5)
                            Nodo *huirLenador = crearEvento(67, "Tomar una gallina y salir corriendo (AGI 5)", "Decides que enfrentar al leñador no es la mejor opción y optas por huir. Corres lo más rápido que puedes, esquivando árboles y saltando sobre raíces. El leñador te sigue de cerca, gritando amenazas mientras intenta alcanzarte. Finalmente, logras perderlo de vista cuando llegas a una pradera y te detienes a descansar en un sitio seguro.\n", statsDefault, 7);
                            huirLenador->condiciones = condicionAgilidad2;
                            agregarHijo(robarGallina, huirLenador);

                                // Nodo 8A.A.B.B.B.A: Comer gallina
                                Nodo *comerGallina= crearEvento(68, "Comer la gallina", "Después de todo lo que hiciste, ya no te puedes aguantar las ganas y empiezas a devorar la gallina entera, con plumas y todo. Quedas satisfecho y disfrutas el sol, pero de repente, un dolor insufrible te afecta. Te das cuenta de que algo anda muy mal.\n\nTe retuerces de dolor, sintiendo que algo te quema por dentro. El dolor se expande desde el estomago y se intensifica rápidamente, empezando a sudar frío y corto de aliento. Intentas buscar ayuda, pero te resulta imposible moverte. La visión se te nubla y, con un último suspiro, todo se vuelve oscuro.\n\nTe das cuenta demasiado tarde de que la gallina estaba infectada con una enfermedad mortal. Tu aventura termina aquí.\n", statsDead, 8);
                                agregarHijo(huirLenador, comerGallina);

                                // Nodo 8A.A.B.B.B.B.B: No comer la gallina (LUCK 3)
                                Nodo *noComerGallina = crearEvento(69, "Dejar vivir a la gallina (LCK 3)", "Decides que es mejor no arriesgarte a comer la gallina que has robado, es muy tierna para matarla y no podrias vivir con la culpa. Aunque tienes hambre, optas por seguir adelante y buscar otra fuente de comida, pero te llevas a la gallina para que te haga compañia. Desde ahora, empieza un nuevo capítulo con tu nueva mascota, y mientras caminan por la pradera, se imaginan que aventuras les depara.\n\nEl fin :D.\n", statsDefault, 8);
                                noComerGallina->condiciones = condicionSuerte1;
                                agregarHijo(huirLenador, noComerGallina);
                        
        // Nodo 2A.B: Explorar Cueva (BATALLA)
        Player statsLeft_2 = {0};
        statsLeft_2.gold += 15 * jugador.luck; 
        Nodo* startLeft_2 = crearEventoMinijuego(14, "Explorar Cueva", "Cruzas el río para explorar la cueva. Encuentras una espada para defenderte en la entrada al lado de un esqueleto. Tomas la espada y sigues explorando hasta encontrar una bolsa con 50 de oro. Celebras por un segundo hasta que escuchas un gruñido afuera de la cueva. Sales a explorar y te encuentras con un ogro esperándote, no tienes otra opción que luchar.\n", statsLeft_2, enemigoOgro);
        agregarHijo(startLeft, startLeft_2);

        // Nodo 2A.B: Despues de Batalla (DIALOGO)

        Nodo* encuentroVendedor1 = crearEvento(30, "Continuar", "Después de matar al ogro, sigues el rio y en la esquina de las rocas sale un vendedor. Los dos están sorprendidos de encontrarse, pero inmediatamente, el vendedor actúa amable y te muestra los suministros que carga en su mochila. Revisas las monedas que encontraste pero antes de negociar, intentas hablar con él\n", statsDefault, 3);
        agregarHijo(startLeft_2, encuentroVendedor1);

            // NODO 3A.B.A: Pedir direcciones
           char mensajeDialogo1[512]; // Asegúrate de que el tamaño sea suficiente para el mensaje completo
           // Usar sprintf para formatear el mensaje
           sprintf(mensajeDialogo1, "-Hey, por favor, tienes que ayudarme. Me llamo %s y me acaba de atacar ese monstruo. No sé donde me encuentro, sabes donde está la aldea más cercana?-", jugador.name);
           Nodo* dialogoVendedor1 = crearEvento(31, mensajeDialogo1, "A pesar de tu desesperación, el vendedor te ignora y no responde ni un sonido. En vez de ofrecerte ayuda, extiende su mano y pide oro. Le miras feo y tratas de seguir pidiendo ayuda, pero su mano izquierda sigue con el gesto mientras que la otra levanta 5 dedos\n", statsDefault, 3);
           agregarHijo(encuentroVendedor1, dialogoVendedor1);

            // NODO 3A.B.B: Pedir ayuda
            Nodo* dialogoVendedor2 = crearEvento(32, "-Hey, ayudame, necesito comida y cura, tuve una pelea con ese ogro y casi me mata!-", "Tratas de mantener la calma y priorizar tu salud, pero el vendedor tiene poca empatía y no responde. Aunque parece considerar la idea de ayudarte, el vendedor primero mira a su alrededor, extiende su mano y pide oro.\n", statsDefault, 3);
            agregarHijo(encuentroVendedor1, dialogoVendedor2);

                // NODO 4A.B.B.A: Pagar por ayuda
                Player stats_payVendor = {0};
                stats_payVendor.gold -= 10;
                stats_payVendor.health += 20;
                Nodo* dialogoVendedor2A = crearEvento(37, "Pagar 10 de oro por ayuda", "Con un suspiro, te rindes y no tienes de otra que pagarle al vendedor, el cuál se ríe mientras te pasa una poción de cura y un pan simple. Comes y tomas de mala gana, pero por lo menos te sientes mejor, aunque tu cara no lo muestra. El vendedor se da cuenta y decide retirarse antes de que te arrepientas de tu decisión, y aunque tenías mas dudas, decides no seguirlo ya que se dirige hacia el bosque\n",  stats_payVendor, 4);
                dialogoVendedor2A->condiciones = condicionDinero3;
                agregarHijo(dialogoVendedor2, dialogoVendedor2A);

                // NODO 4A.B.B.B: Convencer por ayuda (CH 3)
                Player stats_convinceVendor = {0};
                stats_convinceVendor.health += 50;
                Nodo* dialogoVendedor2B = crearEvento(38, "Convencer por ayuda (CH 3)", "No quieres pagarle al vendedor, pero sabes que puedes razonar con él, así que respiras hondo y tratas de calmarte para persuadirlo.\n\n-Mira, sé que quieres hacer tu negocio y todo eso, pero deberías estar agradeciendome. Ni te das cuenta de donde caminas y te hubieras topado con ese ogro. Si no fuera por mí, el tu serías el que hubiera acabado muerto.. Tienes suerte de que no tengo las ganas de hacerte lo mismo, así que enseña un poco de gratitud, no seás codicioso-\nDespués de escuchar tu experiencia, el vendedor se ve forzado por su miedo a ayudarte, por lo que te pasa una poción de cura que no dudas en tomar, además de un amuleto de suerte y comida de sobra. Además, el vendedor te pasa un mapa del área local y señala puntos de interés, como el bosque cercano, un valle entre un conjunto de montañas y una aldea no tan lejana, todo conectada por un sendero que se encuentra más adelante\n\nTe tomas tu tiempo para recordar el mapa, y después de ponerte el amuleto, sientes que tu día va a seguir mejorando\nGanaste 50 puntos de vida y 1 punto de suerte.\n",  stats_convinceVendor, 4);
                dialogoVendedor2B->condiciones = condicionCarisma1;
                agregarHijo(dialogoVendedor2, dialogoVendedor2B);

                // NODO 4A.B.B.C: Amenazar
                Nodo* dialogoVendedor2C = crearEvento(39, "Amenazar a que te ayude", "-Escucha me bien, viejo degenerado. Si no me pasas algunos de tus suministros, lo vas a arrepentir. Ves a ese ogro de allá, pues adivina qúe, él también trato de estafarme, así que a menos de que no se te ocurra pedirme plata, te la vas a ver conmigo-\n\nEl vendedor se congela y te mira incredulo antes de romperse a carcajadas y tratar de ignorarte. Te acobardas con la verguenza, pero como el vendedor trata de irse, te das la vuelta y ves la oportunidad de compromoterte a tu palabra\n",  statsDefault, 4);
                agregarHijo(dialogoVendedor2, dialogoVendedor2C);

                    // NODO 5A.B.B.C.A: Golpear mientras no mira (3 Str)
                    Player stats_suckerPunch = {0};
                    stats_suckerPunch.health += 50;
                    stats_suckerPunch.luck += 1;
                    stats_suckerPunch.gold += 300 * (1+(2*(jugador.luck/10)));
                    Nodo* suckerPunch = crearEvento(40, "Golpearlo en la nuca (STR 3)", "Aprovechas a que te da la espalda para pegarle lo más fuerte posible al vendedor. Tu golpe conecta perfectamente con su cuello con suficiente fuerza para fracturarlo, dejandolo paralizado inmediatamente. Tu propia fuerza te asombra, pero como el vendedor ahora está en el suelo, retorciendose como una oruga, aprovechas a robarle todas sus pertenencias\n\n Encuentras una poción de cura que no dudas en tomar, además le robas un amuleto de suerte y toda la comida. El vendedor parece que no puede repsirar, pero mientras se ahoga, revisas su mochila y encuentras una gran bolsa de oro. Para tu suerte, también encuentras un mapa del área local con puntos de interés, como el bosque cercano, un valle entre un conjunto de montañas y una aldea no tan lejana, todo conectada por un sendero que se encuentra más adelante\n\nTe tomas tu tiempo para recordar el mapa, y después de ponerte el amuleto, sientes que tu día va a seguir mejorando, no como la del vendedor\nGanaste 50 puntos de vida y 1 punto de suerte y una gran bolsa de oro\n",  stats_suckerPunch, 5);
                    suckerPunch->condiciones = condicionFuerza1;
                    agregarHijo(dialogoVendedor2C, suckerPunch);
                    
                    // NODO 5A.B.B.C.B: Luchar vendedor (COMBATE)
                    Player stats_fightVendor = {0};
                    stats_fightVendor.health -=10;
                    Nodo* fightVendor = crearEventoMinijuego(41, "Luchar al vendedor", "Enfurecido por el vendedor, lo tackleas al suelo tratadando de robar su mochila con suministros. El bastardo se resiste y te empuja al río antes de sacar una daga para atacarte. Alcanza a rozar tu piel y cortarte, pero sacas tu espada para defenderte.\nPerdiste 10 puntos de vida\n", stats_fightVendor, enemigoVendedor);
                    agregarHijo(dialogoVendedor2C, fightVendor);

                    // NODO 5A.B.B.C.A: Seguir vendedor
                    Nodo* dontFightVendor = crearEvento(42, "Seguir al vendedor en secreto", "Como no te va a ayudar, decides dejar al vendedor en paz, o por lo menos quieres que piense eso, ya que apenas entra de nuevo al bosque, tú lo sigues, utilizando la vegetación para esconderte. Eventualmente llegan a una colina despejada que conecta a un sendero rocoso, donde ya no te puedes esconder, así que esperas a que el vendedor avance y veas a que dirección se dirige.\n\nPero apenas el vendedor sale de tu campo de visión, lo escuchas gritar afligido y después a sollozar con dolor. La curiosidad te lleva a seguir el sendero y ver que le habrá pasado, pero te encuentras con una banda de tres ladrones dandole una paliza al vendedor.\n\nTratas de irte antes de que se den cuenta de tu presencia, pero ya es muy tarde, porque uno de los ladrones te dice -Hey, hey, hey, no tan rápido. A donde crees que vas?-\nTe das la vuelta y ves que es muy tarde para escapar, y como ven que estás armado, te tomas un segundo para mirar al vendedor suplicandote con sus ojos hinchados. Respiras hondo y luego enfrentas a los ladrones con confianza, diciendoles...\n",  statsDefault, 5);
                    agregarHijo(dialogoVendedor2C, dontFightVendor);

                    // Nodo 6A.B.B.C.A.A: Defender al vendedor
                    Nodo* defenderVendedor = crearEventoMinijuego(70, "Defender al vendedor", "Decides que no puedes dejar que el vendedor sufra más. Con valentía, te interpones entre los ladrones y el vendedor, sacando tu arma.\n -¡Déjenlo en paz!- gritas, atrayendo la atención de los ladrones. Se ríen y uno de ellos avanza hacia ti con su arma en alto. -¡Muy bien, héroe! Veamos si puedes manejar esto!-.\n", statsDefault, enemigoLadron);
                    agregarHijo(dontFightVendor, defenderVendedor);
                
                    // Nodo 6A.B.B.C.A.B: Negociar con los ladrones
                    Nodo* negociarLadrones = crearEvento(71, "Negociar con los ladrones", "Decides que luchar no es la mejor opción y optas por intentar razonar con los ladrones. Levantas las manos en un gesto de paz y dices -Esperen, no tiene por qué ser así. Este vendedor no tiene mucho, y si lo dejan ir, puede incluso pagarles más tarde. ¿Por qué no arreglamos esto de manera civilizada?-.\n", statsDefault, 6);
                    agregarHijo(dontFightVendor, negociarLadrones);
                
                        // Nodo 7A.B.B.C.A.B.A: Convencer a los ladrones (CH 5)
                        Nodo* convencerLadrones = crearEvento(72, "Convencer a los ladrones (CH 5)", "Tu tono calmado y tus palabras lógicas parecen hacer mella en los ladrones. Después de un tenso momento de silencio, el líder de los ladrones asiente lentamente. -Está bien,- dice. -Le daremos una oportunidad. Pero si nos traiciona, pagará el doble.-. Los ladrones retroceden, permitiendo que el vendedor se ponga de pie, tembloroso pero agradecido.\nJuntos se retiran de la escena para descansar después del susto que pasaron, y desde ese día formaron una amistad que duró dos días ya que los ladrones regresaron y los mataron.\n\nEl fin\n", statsDefault, 7);
                        convencerLadrones->condiciones = condicionCarisma2;
                        agregarHijo(negociarLadrones, convencerLadrones);
                    
                        // Nodo 7A.B.B.C.A.B.B: Distraer a los ladrones
                        Nodo* distraerLadrones = crearEvento(73, "Distraer a los ladrones", "A pesar de tus mejores esfuerzos, los ladrones no parecen interesados en hablar. -¡Basta de palabras!- grita uno de ellos, persiguiendote de inmediato.\n", statsDefault, 7);
                        agregarHijo(negociarLadrones, distraerLadrones);

                            // Nodo 8A.B.B.C.A.B.B.A: Escapar (AGI 5)
                            Nodo* escaparLadrones = crearEvento(75, "Correr y alejar a los ladrones del vendedor (AGI 5)", "Antes de que te toquen, sales corriendo adelante sin saber a donde vas. Sin mirar atrás, corres por tu vida, y aunque no viste al vendedor, sientes los tres ladrones detrás tuyo, así que sigues corriendo y corriendo hasta el fin del mundo...\n", statsDefault, 8);
                            escaparLadrones->condiciones = condicionAgilidad2;
                            agregarHijo(distraerLadrones, escaparLadrones);

                            // Nodo 8A.B.B.C.A.B.B.B: Defender al vendedor
                            Nodo* enfrentarLadrones = crearEventoMinijuego(76, "Pelear ladrones mientras el vendedor escapa", "Te ves obligado a defenderte mientras el conflicto escala rápidamente. Esquivas el primer golpe y retrocedes para darle espacio al vendedor, pero en vez de los tres ladrones atacandote, el más grande te enfrenta mientras que los otros dos mantienen al vendedor en su lugar. Parece que no podrás distraer a los ladrones, pero por lo menos los vas a entretener al darle una lección a su lider\n", statsDefault, enemigoLadron);
                            agregarHijo(distraerLadrones, enfrentarLadrones);
                
                    // Nodo 6A.B.B.C.A.C: Ayudar a los ladrones
                    Nodo* ayudarLadrones = crearEventoMinijuego(74, "Ayudar a los ladrones a robar al vendedor", "Después de un momento de reflexión, decides que quizás es mejor aliarte con los ladrones. -¿Sabes qué?- dices con una sonrisa. -Creo que podríamos trabajar juntos. Este vendedor tiene más de lo que parece.-. Los ladrones se miran entre sí, sorprendidos pero intrigados, y luego asienten. Te unes a ellos para despojar al vendedor de sus pertenencias. Pero cuando terminan, te miran y demandan -Ahora matalo, no queremos que nos reporte a alguien, ya nos vio las caras-\n\nDudando, miras al vendedor moribundo y decides acabar con su sufrimiento ya que ni puede caminar", statsDefault, enemigoVendedor2);
                    agregarHijo(dontFightVendor, ayudarLadrones);


            // NODO 3A.B.C: Pasarse de cazador de ogros (3 CH)

            Nodo* dialogoVendedor3 = crearEvento(33, "Hey, sabes donde hay mas ogros? Estoy de caza y acabo de matar a uno, te podria venderterlo o intercambiarlo por algunos de tus suministros ¿Qué te parece? (CH 3)", "Tu oferta agarra la atención del vendedor, el cual instinctivamente se asoma para revisar al ogro muerto y saca su bolsa de monedas de oro para contarlas mientras tú miras con ansiosamente.\n", statsDefault, 3);
            dialogoVendedor3->condiciones = condicionCarisma1;
            agregarHijo(encuentroVendedor1, dialogoVendedor3);

                // NODO 4A.B.C.A: Vender ogro por dinero
                Player stats_sellOgre = {0};
                stats_sellOgre.gold += 50;
                Nodo* dialogoVendedor3A = crearEvento(34, "Vender por 50 oro", "Con una sonrisa en la cara y codicia en los ojos, el vendedor te pasa 50 monedas de oro.\n",  stats_sellOgre, 4);
                agregarHijo(dialogoVendedor3, dialogoVendedor3A);

                // NODO 4A.B.C.B: Robar bolsa oro (AGI 3)
                Player stats_stealVendor = {0};
                stats_stealVendor.gold += 500;
                Nodo* dialogoVendedor3B = crearEvento(35, "Robar bolsa (AGI 3)", "Mientras el vendedor está distraido contando las monedas, agarras la bolsa y empiezas a correr sin pensarlo. Con el vendedor persiguiendote, sigues recto, siguiendo el río hasta que ganas suficiente distancia para detenerte en una intersección antes de un valle.\nA la derecha se encuentra un camino de tierra vacío que lleva a las colinas y bordea el bosque, mientras que a la izquierda se encuentra un camino más oscuro que parece no tener salida. Te detienes a pensar, pero el vendedor se está acercando rápido\n",  stats_stealVendor, 4);
                dialogoVendedor3B->condiciones = condicionAgilidad1;
                agregarHijo(dialogoVendedor3, dialogoVendedor3B);
    
                    // NODO 5A.B.C.B.A: Luchar
                    Nodo* lucharVendedor = crearEventoMinijuego(54, "Enfrentarte al vendedor", "Decides que la mejor opción es enfrentar al vendedor y poner fin a la persecución. Te giras rápidamente, con la bolsa de oro firmemente sujeta en tus manos y una sonrisa macabra antes de lanzarte a la batalla\n",  statsDefault, enemigoVendedor);
                    agregarHijo(dialogoVendedor3B, lucharVendedor);

                    // NODO 5A.B.C.B.B: Ir hacia colinas
                    Nodo* runToHills = crearEvento(55, "Ir hacia colina", "Decides correr hacia el camino de tierra que lleva a las colinas. Sabes que el terreno elevado puede ofrecerte alguna ventaja y tal vez un lugar seguro para esconderte. Corres tan rápido como puedes, sintiendo el peso de la bolsa de oro en tus manos, pero antes de alcanzar la colina más cercana, el vendedor atrae la atención de un grupo de guerreros que sale del bosque. Te das la vuelta y miras como tres individuos ahora se dirigen hacia ti, corriendo más rapido que tú y demandando que te detengas.\n\n Por el sombrero y el bastón mágico que tiene en la mano, identificas a una hechichera que está detrás con el vendedor, pero hay una arquera elfa rubia apuntado su arco, mientras que un guerrero de pelo corto te persigue con una espada más grande que su cuerpo, pero aún así te alcanza y está apunto de atacar\n",  statsDefault, 5);
                    agregarHijo(dialogoVendedor3B, runToHills);

                        // NODO 6A.B.C.B.B.A: Convencer que vendedor te quiere matar (CH 3)
                        Nodo* lieToGroup = crearEvento(57, "Convencer que vendedor te quiere matar (CH 3)", "\n",  statsDefault, 6);
                        lieToGroup->condiciones = condicionCarisma1;
                        agregarHijo(runToHills, lieToGroup);

                        // NODO 6A.B.C.B.B.B: Ir hacia colinas
                        Nodo* fightGroup = crearEventoMinijuego(58, "Pelear con grupo", "\n",  statsDefault, enemigoThorne);
                        agregarHijo(runToHills, fightGroup);

                        // NODO 6A.B.C.B.B.C: Eres salvado (LUCK 5)
                        Nodo* saved= crearEvento(59, "Seguir corriendo y rezar por un milagro (LCK 3)", "No puedes pensar y solo sigues corriendo, cerrando tus ojos en anticipación de tu muerte. Sientes flechas volando sobre ti, con el jadeo del guerrero persiguiendote causando más desesperación. Pero justo cuando llegas a la colina, sientes un trueno al lado tuyo que te arrebata y lleva al suelo, igual que al guerrero.\n\nNo quieres abrir los ojos, pero luego escuchas al grupo gritar en agonia mientras que un personaje misterioso se rie sin preocupaciones. Te levantas y tratas de descifrar que ha pasado, y es ahí cuando te das cuenta que alguien vino a ayudarte\n",  statsDefault, 6);
                        saved->condiciones = condicionSuerte1;
                        agregarHijo(runToHills, saved);
    
                        // NODO 7A.B.C.B.B.C.A: Goku ending
                        Nodo* goku = crearEvento(60, "Es Goku", "De los cielos, en toda su gloria y belleza, Goku SSJ 2 desciende y asesina a todo el grupo con una genki-dama, dejando solo al vendedor vivo para poder tormentarlo, acercandose tenebrosamente con una sonrisa en la cara antes de decir -Sabes, el destino es una criatura caprichosa. Te trae hasta aquí, a mi presencia, como un cordero al matadero.\n\n¿Sientes ese escalofrío recorriendo tu espalda? No es solo el frío de esta sala. Es el susurro del miedo, esa sombra oscura que siempre ha estado contigo, acechando en los rincones más profundos de tu mente.\n\nMuchos han intentado desafiarme, todos han caído. No porque sean débiles, no. Sino porque no entienden el verdadero poder que manejo. El poder que no se ve, que no se oye, pero que se siente en cada respiración, en cada latido.\n\nTu viaje termina aquí, Freezer. No hay salvación, no hay esperanza. Solo yo, y la oscuridad que te envuelve mientras tu envidia y locura te consume-\nConfundido, el vendedor murmura -Me llamo Sergio, y...- pero antes de que pueda decir otra palabra, Goku activa Super Saiyajin 28 y aniquila al vendedor.\n\nQuedaste sin palabra, agradecido pero a la vez aterrorizado por lo que acabas de ver, pero Goku ahora se da la vuelta y te mira directamente a los ojos, su mirada penetrante tocandote y manoseando tu corazón. No sabes que hacer hasta que se para enfrente tuyo y te agarra de cerca, susurrandote al oido -Estás bien?-\nA lo que solo puedes responder sonrojando y diciendo -Gracias, Goku-chan, eres mi heroe-\n\nLos dos se besan apasionadamente, y después de cinco minutos disfrutando sus labios, te sonríe y lleva volando por los aires, admirando su alrededor antes de desaparecer en el espacio, donde continuarán una vida perfecta ahora que Freezer fue derrotado\n\nEl fin :D\n",  statsDefault, 7);
                        agregarHijo(saved, goku);
    
    
                    // NODO 5A.B.C.B.C: Ir por camino del rio
                    Nodo* runAlongRiver = crearEvento(56, "Ir hacia el camino oscuro", "Optas por tomar el camino más oscuro, con la esperanza de que la densa vegetación y la falta de visibilidad te ayuden a perder al vendedor. Corres a través del sendero estrecho, con el corazón latiendo a toda velocidad. A medida que te adentras más en la oscuridad, el sonido del río se vuelve más fuerte, guiándote a través del terreno incierto hasta una laguna, donde inmediatamente saltas para esconderte sin percatarte de que adelante se encuentra una granja.\n\nEl vendedor perdío tu rastro y ves como sigue corriendo por el camino sin darse cuenta de que te escondiste, pero se detiene a la puerta de la granja, donde toca la puerta con desesperación y pide ayuda.\n\nTratas de quedarte quieto en la laguna, pero ves que un leñador enorme sale de su granja y habla con el vendedor, quién inmediatamente pide revisar el alrededor. Ves que se dirigen a la laguna, pero aún no te han visto, aunque sería inevitable a menos de que te sumergas y aguantes sin respirar hasta que se vayan\n",  statsDefault, 5);
                    agregarHijo(dialogoVendedor3B, runAlongRiver);

                        //NODO 6A.B.C.B.C.A: Emboscar al leñador y pelear
                        Nodo* ambushLumberjack = crearEventoMinijuego(63, "Emboscar al leñador y pelear", "Decides que la mejor defensa es un buen ataque. Mientras el vendedor y el leñador se acercan a la laguna, te preparas para una emboscada. Saltas del agua y atacas al leñador, esperando sorprenderlo y ganar la ventaja.\n\nEl leñador, aunque sorprendido, reacciona rápidamente y se prepara para defenderse. La pelea se intensifica rápidamente, y te das cuenta de que este no será un enfrentamiento fácil.Te preparas para una batalla feroz, sabiendo que tu vida depende de ello.\n", statsDefault, enemigoLeñador);
                        agregarHijo(runAlongRiver, ambushLumberjack);

                        //NODO 6A.B.C.B.C.B: Aguantar respiracion (END 5)
                        Nodo* holdBreath = crearEvento(64, "Aguantar respiración en agua (END 5)", "Decides que la mejor opción es sumergirte y aguantar el aliento hasta que se vayan. Con cuidado, te deslizas bajo el agua, sintiendo el frío envolverte mientras contienes la respiración.\n\nPuedes escuchar sus voces cada vez más cerca, y tu corazón late con fuerza mientras luchas por mantenerte oculto. El leñador y el vendedor se detienen justo en la orilla de la laguna, sus sombras oscuras proyectándose sobre el agua.\n\nFinalmente, después de lo que parece una eternidad, se alejan, sin haberte descubierto. Con cuidado, sales a la superficie y respiras profundamente, agradecido por tu resistencia\nCuidadosamente sales de la laguna, y esperas a que el leñador regrese adentro de la granja para correr con el oro y buscar ayuda en una aldea cercana.\n\nEl fin, no hay más lmao", statsDefault, 6);
                        holdBreath->condiciones = condicionResistencia2;
                        agregarHijo(runAlongRiver, holdBreath);

                        //NODO 6A.B.C.B.C.B: Convencer estafa (CH 5)
                        Nodo* convinceLumberjack = crearEvento(65, "Convencer al leñador que el vendedor lo está estafando (CH 5)", "Decides intentar algo arriesgado. Saliendo lentamente de la laguna, llamas la atención del leñador antes de que él y el vendedor te vean. ¡Espera! Ese hombre te está estafando. Me robó y ahora quiere usar tu ayuda para atraparme. No es de fiar.\n\nEl leñador, confundido, mira al vendedor con sospecha. El vendedor empieza a balbucear una defensa, pero puedes ver la duda en los ojos del leñador.\n\n-Escúchame, solo estoy intentando recuperar lo que es mío. Este hombre es un mentiroso y un ladrón. No te dejes engañar por sus palabras-\n El leñador, después de un momento de consideración, asiente lentamente y gira hacia el vendedor, exigiendo una explicación.", statsDefault, 6);
                        convinceLumberjack->condiciones = condicionCarisma2;
                        agregarHijo(runAlongRiver, convinceLumberjack);
    
    
                // NODO 4A.B.C.B: Intercambiar
                Player stats_tradeVendor = {0};
                stats_tradeVendor.health += 50;
                stats_tradeVendor.luck += 1;
                Nodo* dialogoVendedor3C = crearEvento(36, "Intercambiar por suministros", "Antes de que el vendedor ofrezca sus monedas, lo detienes y le dices ""No será necesario, solo necesito algunos suministros y direcciones para seguir cazando ogros. ¿Te ubicas por aquí?""\n\nAliviado de no tener que gastar sus preciosas monedas, el vendedor te pasa una poción de cura que no dudas en tomar, además de un amuleto de suerte y comida de sobra. Además, el vendedor saca un mapa del área local y señala puntos de interés, como el bosque cercano, un valle entre un conjunto de montañas y una aldea no tan lejana, todo conectada por un sendero que se encuentra más adelante\n\nTe tomas tu tiempo para recordar el mapa, y después de ponerte el amuleto, sientes que tu día va a seguir mejorando\nGanaste 50 puntos de vida y 1 punto de suerte\n",  stats_tradeVendor, 4);
                agregarHijo(dialogoVendedor3, dialogoVendedor3C);
    

    // Nodo 1B: Ir a la derecha
    Nodo *startRight = crearEvento(15, "Ir a la derecha", "Sigues un camino y te mantienes en el bosque hasta que te encuentras con unas bayas misteriosas y un camino despejado adelante.\n", statsDefault, 1);
    agregarHijo(arbol->root, startRight);

        // Nodo 2B.A: Comer bayas
        Player statsRight_1 = {0};
        statsRight_1.endurance += 1; 
        Nodo *startRight_1 = crearEvento(16, "Comer bayas", "Agarras un puñado de bayas para saciar el hambre. Tiene un sabor peculiar, empieza dulce y luego amargo. No puedes resistir el sabor y te atoras. Finalmente, aguantas y tragas las bayas, pero cuando llegan a tu estómago, sientes un dolor agudo que te deja aturdido. Ganas 1 de resistencia.\n", statsRight_1, 2);
        agregarHijo(startRight, startRight_1);

            // Nodo 3B.A.A: Descansar
            Player statsRest = {0};
            statsRest.health += 5; 
            Nodo *berriesRest = crearEvento(17, "Descansar y esperar ayuda", "Descansas y recuperas 5 de salud, pero pierdes tiempo. Te despiertas en el medio de la noche y eres emboscado por un oso salvaje y debes defenderte.\n", statsRest, 3);
            agregarHijo(startRight_1, berriesRest);

                // Nodo 4B.A.A.A: Huir Oso (AGI 3)
                Nodo *huirOso = crearEvento(43, "Huir oso (AGI 3)", "Rápidamente evalúas la situación y decides que huir es la mejor opción. Con movimientos ágiles, te escabulles entre los árboles y tratas de perder al oso en la maleza espesa del bosque. La agilidad de tus movimientos te ayuda a mantenerte delante del oso y finalmente logras perderlo de vista, ganando algo de distancia en tu huida.\n", statsDefault, 4);
                huirOso->condiciones = condicionAgilidad1;
                agregarHijo(berriesRest, huirOso);

                    // Nodo 5B.A.A.A.A: Esconderse en la cueva
                    Nodo *esconderseCueva = crearEvento(90, "Esconderse en una cueva", "Localizas una cueva cercana y decides esconderte dentro. Aunque es oscuro y húmedo, al menos te proporciona un lugar seguro para descansar temporalmente hasta que el oso se aleje o amanezca.\n", statsDefault, 5);
                    agregarHijo(huirOso, esconderseCueva);
                
                    // Nodo 5B.A.A.A.B: Trepar árbol alto (AGI 3)
                    Nodo *treparArbol = crearEvento(91, "Trepar a un árbol alto (AGI 3)", "Buscas un árbol robusto y trepas lo más alto posible para alejarte del alcance del oso. Desde arriba, puedes observar mejor el entorno y planificar tu próximo movimiento con mayor seguridad.\n", statsDefault, 5);
                    treparArbol->condiciones = condicionAgilidad1;
                    agregarHijo(huirOso, treparArbol);

                    // Nodo 5B.A.A.A.C: Seguir recto y encontrar bifurcación
                    Nodo *continuarOso = crearEvento(92, "Seguir corriendo recto", "Decides seguir recto en lugar de esconderte o trepar a un árbol. Avanzas a través del bosque y eventualmente te encuentras con un acantilado. Ves un rio al fondo, pero es un salto muy grando. No tienes mucho tiempo para pensar\n", statsDefault, 5);
                    agregarHijo(huirOso, continuarOso);

                        // Nodo 6B.A.A.A.C.A: Saltar acantilado
                        Nodo *jumpCliff = crearEvento(93, "Saltar al agua", "Decides saltar, poniendo toda tu fé en tus piernas para hacer un salto largo que alcance a llevarte al agua. Caes más rápido de lo que piensas y mientras el suelo se acerca, cierras los ojos y confias en los angeles protegiendote. Desafortunadamente, la jornada nocturna de los ángeles terminó hace dos horas y ya no resguardan tu vida, por lo que incluso cuando caes en el agua, esta solo tiene una profundidad de medio metro, por lo que falleces instantaneamente en el impacto. Rip, si tan solo hubieras saltado dos horas antes\n", statsDead, 6);
                        agregarHijo(continuarOso, jumpCliff);

                        // Nodo 6B.A.A.A.B: Tropezar
                        Nodo *trickBear = crearEvento(94, "Doblar y correr por la orilla (LCK 3)", "Decides hacer un giro al ultimo minuto para tratar de despistar al oso, pero terminas tropezandote justo en el borde del acantilado. Te cubres la cabeza y anticipas la muerte, pero cuando el oso ataca, este sigue de largo y cae 50 metros hacia su muerte en el fondo del acantilado. Te paras y sigues con tu camino como si nada\n", statsDefault, 6);
                        trickBear->condiciones = condicionSuerte1;
                        agregarHijo(continuarOso, trickBear);

                // Nodo 4B.A.A.B: Luchar Oso (COMBATE)
                Enemy* enemigoOso = crearEnemigo("Oso", 55, 2); 
                Nodo* lucharOso = crearEventoMinijuego(44, "Luchar oso", "Antes de que te puedas parar, el oso salta y trata de morderte. Logras esquivarlo y pararte, pero no tienes otra opción que luchar por tu vida", statsDefault, enemigoVendedor);
                agregarHijo(berriesRest, lucharOso);

                Nodo* afterBearFight = crearEvento(95, "Continuar", "Después de derrotar al oso, sigues un rastro que dejó que te lleva a una cueva. Te das cuenta de que te encuentras en su guarida, y parece que puede haber algo de valor\n", statsDefault, 4);
                agregarHijo(lucharOso, afterBearFight);

                // Nodo 5B.A.A.B.A: Explorar la guarida del oso
                Nodo *explorarGuarida = crearEvento(96, "Explorar la guarida del oso", "Te adentras en la cueva y empiezas a buscar alrededor, te encuentras con ropa desgarrada y rastros de sangre que no inspiran mucha confianza, pero te sigues adentrando hasta que te encuentras a los bebes del oso.\n", statsDefault, 5);
                agregarHijo(afterBearFight, explorarGuarida);

                    // Nodo 6B.A.A.B.A.A: Matar bebes
                    Player statsKillBabies = {0};
                    statsKillBabies.health += 30;
                    statsKillBabies.endurance += 3;
                    statsKillBabies.strenght += 3;
                    Nodo *killBabies = crearEvento(98, "Matar y comer ositos bebes", "Cual es tu problema, en serio, que te pasa. Ni siquiera hay una pelea, no pueden pelear, tienen como 3 meses. Tal vez el verdadero monstruo siempre ha sido tú. Celebra tu victoria contra ositos tiernitos indefensos. Ganaste 30 puntos de vida, 3 puntos de resistencia, y 3 puntos de fuerza, espero que haya sido divertido.\n", statsKillBabies, 6);
                    agregarHijo(explorarGuarida, killBabies);

                    // Nodo 6B.A.A.B.A.A: Adoptar osos
                    Nodo *mamaBear = crearEvento(99, "Adoptar bebes y absolve el pecado de matar a una madre", "Has encontrado tu nuevo proposito en tu vida, y desde ahora vas a cuidar, amar, y tratar a estos ositos como tu familia. De hecho, ahora te conviertes en un oso. Listo, final feliz :)\n", statsDefault, 6);
                    agregarHijo(explorarGuarida, mamaBear);


                // Nodo 5B.A.A.B.B: Continuar el camino
                Nodo *ignoreBear = crearEvento(97, "Continuar tu camino", "Prefieres no detenerte y sigues adelante después de la pelea con el oso, buscando una salida o el siguiente paso en tu viaje. Sigues recto hasta que te encuentras con acantilado que bloquea tu camino, por lo que recorres el borde hasta encontrar un lugar de interes.\n", statsDefault, 5);
                agregarHijo(afterBearFight, ignoreBear);

    
    
            // Nodo 3B.A.B: Seguir caminando
            Nodo *ignoreStomachAche = crearEvento(18, "Ignorar malestar y seguir", "Decides ignorar el malestar y continúas con tu camino. Te das cuenta que te desviaste y no seguiste recto, pero en vez de regresar, te acercas un poco más a una fuente de sonido cercana. Sigues el camino equivocado hasta encontrar un campamento que parece estar desolado.\n", statsDefault, 3);
            agregarHijo(startRight_1, ignoreStomachAche);

                // Nodo 4B.A.B.A: Explorar Campamento
                Nodo *explorarCampamento = crearEvento(45, "Explorar campamento", "Saliendo del bosque y entrando al desolado campamento, lo primero que haces es buscar ayuda después de haberte perdido. A medida que te acercas, el campamento se revela ante ti: una colección de chozas de madera rudimentarias dispersas alrededor de un pequeño claro. El lugar parece haber sido abandonado recientemente, pero aún así se siente activo, con rastros de fogatas humeantes y herramientas de trabajo tiradas por el suelo.\n\nDecides llamar por ayuda, esperando encontrar algún rastro de civilización que te pueda orientar. Al principio, no obtienes respuesta, pero luego escuchas el crujido de ramas y el murmullo de voces acercándose desde una de las chozas más grandes. Un grupo de guerreros emerge, llevando armaduras de cuero gastadas y armas simples, pero efectivas. Entre ellos, destaca un hombre más alto y robusto que los demás, con una mirada severa y una cicatriz prominente en su rostro: el jefe del campamento.\nSin introducirse o decir una palabra, ordena a sus guerreros a que te capturen y pongan en una jaula de madera cerca del borde trasero del campamento. No tienes tiempo para reaccionar y sabes que no puedes pelear, pero mientras te quejas y demandas una explicación, los guerreros te ignorar y continuan con su rutina, siguiendo al jefe dentro de la choza central, ya que parece que estaban teniendo una reunión o conversación importante\n\nTe dejan solo con dos guardias y tienes un moment para pensar en que hacer antes de que el jefe y sus guerreros desaparezcan\n", statsDefault, 4);
                agregarHijo(ignoreStomachAche, explorarCampamento);
                agregarHijo(backForestRes, explorarCampamento);

                    // Nodo 5B.A.B.A.A: Escapar jaula (STR 3)
                        Nodo *escaparJaula = crearEvento(84, "Escapar de la jaula (STR 3)", "Decides pretender aceptar tu castigo y esperas a que el jefe desaparezca para empezar a formar un plan. Miras alrededor de la jaula y te percatas de que no es muy robusta, solo son barras de madera y una cerradura de hierro que tiene cerrada la puerta. Aprovechas un descuido de los guardias y logras doblar las barras de madera para escapar la jaula, moviéndote sigilosamente para evitar ser visto. Ahora debes encontrar una salida segura del campamento.\n", statsDefault, 5);
                        escaparJaula->condiciones = condicionFuerza1;
                        agregarHijo(explorarCampamento, escaparJaula);
    
                    // Nodo 5B.A.B.A.B: Hablar con jefe
                        Nodo *hablarJefe = crearEvento(85, "Llamar la atención del jefe", "Antes de que se vaya, llamas al jefe de los guerreros y logras captar su atención por un segundo. Es un hombre imponente con una mirada que refleja años de liderazgo y experiencia en la batalla, por lo que piensas bien en que decir. La tribu está reunida detrás de él y siguen discutiendo animadamente un plan para enfrentar un problema que han estado enfrentando con una misión reciente.\n- Permíteme hablar. Este es un malentendido, no tengo motivos para estar acá aparte de encontrar ayuda. Me perdí en el bosque y terminé aquí, pero no soy un enemigo. Claro, no me van a creer, pero como todos estamos en apuro aquí, lo haré breve. He escuchado sus preocupaciones y veo que tienen un desafío con esa misión que están discutiendo. Ya que tenemos este dilema, sugiero que ambos podríamos ayudarnos a superar ese obstáculo, juntos- propones con simpatía, lo cual te da la oportunidad de hablar con el jefe de cerca después de que este se acerca y es impresionado por tu franqueza, aunque no parece muy convencido\n\n-Nisiquiera sabes cuál es nuestro problema, por qué crees que tú nos serías útil- el jefe responde con un tono degradante que te deja pensando y dudando en que decir.\n\n Miras tu alrededor y tratas de buscar pistas que te ayuden a encontrar una solución de este conflicto, pero te das cuenta que el jefe tiene las llaves de la jaula y no está muy lejos, solo necesitas distraerlo y hacer que se acerque un poco mpas. Al mismo tiempo, notas que todos sus guerreros parecen tensos, con expresiones preocupadas y murmullos constantes entre ellos, lo que te da una idea.", statsDefault, 5);
                        agregarHijo(explorarCampamento, hablarJefe);

                        // Nodo 6B.A.B.A.B.A: Convencer a que te dejen ir (CH 3)
                        Nodo *convencerJefe = crearEvento(86, "Convencer al jefe de que te deje ir (CH 3)", "-Bueno, por lo que parece, ninguno de tus subditos quiere ser parte de lo que sea que están tramando, así que podrías usar a alguien que no sea un cobarde- dices de manera burlesca, lo cual ofende al jefe, pero este se da cuenta de que tienes razón\n\n-Okay, pero por qué debería confiar en tí, huh?- el jefe pregunta con sospecha\n-Sinceramente, ahora mismo haría cualquier cosa para comer...- admites con un suspiro que saca una risita del jefe. Él te sigue mirando criticamente, pero después de un momento, asiente y prepara las llaves para liberarte de la jaula, aunque primero te pregunta -Te ayudaremos siempre y cuando no sepas cuál es nuestro objetivo-\n-Eso es un poco sospechoso, cómo sabré que tengo que hacer entonces?- respondes de manera sarcastica\n-Sólo te daremos las direcciones y quiero que entregues un paquete de nuestra parte. Crees que puedes hacerlo?- el jefe pregunta con anhelo y te tomas un segundo en pensarlo. Las reacciones de los guerreros te hacen dudar, pero como ya estás a punto de ser liberado, decides aceptar la misión.\nSolo dime a donde ir e iré de inmediato...-\n\nSatisfecho con tu respuesta, el jefe te libera de la jaula, te invita a una de las chozas, te introuduce a la tribu, y luego de una hora de prepararse y comer, te embarcas en una aventura para entregar un paquete de perlas brillantes a una montaña lejana\n", statsDefault, 6);
                        convencerJefe->condiciones = condicionCarisma1;
                        agregarHijo(hablarJefe, convencerJefe);

                        // Nodo 6B.A.B.A.B.B: Robar llaves (3 Agi)
                        Nodo *robarLlaves = crearEvento(87, "Engañar a jefe y robar llaves (AGI 3)", "Decides actuar rápidamente. Utilizas cualquier distracción o descuido momentáneo para abalanzarte sobre el jefe cuando se acerca lo suficiente, arrebatándole las llaves de la jaula.\n\nLa sorpresa y la rapidez de tu acción deja a todos atontados, lo que te permiten liberarte antes de que los guerreros reaccionen completamente y defiandan a su jefe. Corres hacia la libertad, con el sonido de gritos y confusión detrás de ti mientras te escapas del campamento.", statsDefault, 6);
                        robarLlaves->condiciones = condicionAgilidad1;
                        agregarHijo(hablarJefe, robarLlaves);

                        // Nodo 6B.A.B.A.B.C: Amenazar al jefe y la tribu
                        Nodo *amenazarJefe = crearEvento(88, "Amenazar al jefe y la tribu", "Viendo que no puedes convencer al jefe de manera amigable, decides tomar un enfoque más agresivo. Te pones de pie frente al jefe y su tribu reunida, mostrando tu determinación y voluntad de salir de allí.\n\n-Entiéndanlo bien -dices con voz firme y amenazante-, si no me dejan ir ahora mismo, enviaré un mensaje a mis compañeros. Vendrán con un ejército y arrasarán este campamento hasta el último de ustedes. No me subestimen. Tienen una oportunidad de dejarme ir pacíficamente.\n\nEl jefe y los guerreros se miran entre sí, evaluando tus palabras y la seriedad de tu amenaza. Sabes que estás arriesgando mucho al hacer esta declaración, pero también es tu única opción para evitar una vida de esclavitud o encierro.\n", statsDefault, 6);
                        agregarHijo(hablarJefe, amenazarJefe);
    
                    // Nodo 5B.A.B.A.B: Esperar a que te liberen
                        Nodo *esperarLiberacion = crearEvento(89, "Esperar a que te liberen", "Decides esperar tranquilamente hasta que llegue la noche, confiando en que alguien del campamento o alguna oportunidad de escape se presente mientras todos duermen.\n", statsDefault, 5);
                        agregarHijo(explorarCampamento, esperarLiberacion);
                        
                // Nodo 4B.A.B.B: Ignorar campamento
                Nodo *ignorarCampamento = crearEvento(46, "Regresar al camino", "Decides que el campamento es sospechoso y regresas al camino, ahora siguiendo recto hacia adelante hasta que sales del bosque. Te encuentras con un valle grande y un camino adyacente a un río a tu izquierda, pero mientras estás evaluando tu alrededor, ves a un grupo de tres personas yéndose por el camino, alejándose cada vez más. Como no hay nadie más presente, te tomas tu tiempo para decidir qué hacer..\n", statsDefault, 4);
                agregarHijo(ignoreStomachAche, ignorarCampamento);

        // Nodo 2B.B: Ignorar bayas
        Nodo *startRight_2 = crearEvento(19, "Ignorar bayas", "Decides no comer las bayas y a pesar de tu hambre, sigues el camino adelante hasta que sales del bosque. Te encuentras con un valle grande y un camino adyacente a un río a tu izquierda, pero mientras estás evaluando tu alrededor, ves a un grupo de tres personas yéndose por el camino, alejándose cada vez más. Como no hay nadie más presente, te tomas tu tiempo para decidir qué hacer.\n", statsDefault, 2);
        agregarHijo(startRight, startRight_2);

            // Nodo 3B.B.A: Pedir ayuda al grupo
            Nodo *askHelp = crearEvento(20, "Pedir ayuda", "Antes de que sea muy tarde, corres detrás del grupo, gritándoles por ayuda hasta que se dan la vuelta. Al principio, adoptan la defensa y sacan sus armas, pero después de ver que estás desarmado, te dejan acercarte antes de revisar tu condición.\n\nEl grupo consiste de una hechicera por el sombrero y el bastón mágico que tiene en la mano, un guerrero de pelo corto y marrón con un hacha más grande que su cuerpo entero, y una arquera rubia con orejas largas, por lo que la identificas como una elfa. Solo la hechicera se acerca para ofrecer ayuda, pero antes de pasarte unas vendas y agua, te mira directo a los ojos y pregunta\n-Hey, detente ahí. Dinos quién eres y qué es lo que buscas o tendremos que defendernos-\n", statsDefault, 3);
            agregarHijo(startRight_2, askHelp);
            agregarHijo(ignorarCampamento, askHelp);
    
                // Nodo 4B.B.A.A: Pedir ayuda al grupo
                Player statsgetHelp = {0};
                statsgetHelp.health +=25;
                char dialogoHechizera1[2048]; 
                   sprintf(dialogoHechizera1, "-Me llamo %s, perdone mi impaciencia, pero necesito ayuda. No sé donde me encuentro o cómo llegué aquí, pero siento que no he comido en días y me muero de sed\nExplicas tu situación, haciendo hincapié en tu necesidad desesperada de ayuda y en cómo puedes ser útil para ellos. La hechicera, conmovida por tus palabras y la sinceridad en tus ojos, decide confiar en ti.\n\n-Está bien, pareces sincero. Te ayudaremos, pero debes quedarte con nosotros y seguir nuestras reglas.-\nLa hechicera te pasa una cantina con agua y unas manzanas, las cuáles devoras enfrente del grupo. El guerrero y la arquera no parecen confiar mucho, pero siguen a su compañera, y aunque lo hace de mala gana, el guerrero te pasa una espada que le sobraba. Ganas 25 puntos de vida\n\nDespués de que te has recuperado un poco, Amari, la hechicera, te presenta a los otros miembros del grupo.\n\n-Yo soy Amari- dice la hechicera con una sonrisa. Soy una hechicera de los bosques del norte, siempre en busca de nuevos conocimientos y aventuras.\n-Thorne-, el guerrero, asiente con la cabeza. -Soy Thorne, de las tierras del oeste. He vagado por muchas regiones, protegiendo a los débiles y luchando contra la injusticia.-\nFinalmente, la arquera elfa, se adelanta y te mira con ojos escrutadores y evasivos.-Soy Lyra, del reino de los elfos en el este. Mi puntería es infalible, así que mas te vale no meterte en mi camino\n\nCon las presentaciones hechas, el grupo comienza a moverse, y decides seguirlos, mirando a los tres escéptico pero esperando que esta alianza te ayude en tu viaje. Y justo cuando pasan por la primera colina, su confianza es puesta a prueba, ya que son emboscados por una banda de 4 duendes que aparece de unos arbustos rodeandolos.\nDos de estos enemigos atacan a la elfa, quién iba primero, forzando a la hechicera y el guerrero defenderla, pero le dan la espalda a los otros dos duendes, los cuales al principio te ignoran para aprovecharla oportunidad de atacarlos. Y en ese momento, te encontraste en frente de dos opciones.\n", jugador.name);
                Nodo *hechiceraDialogoA= crearEvento(50, "Ser honesto", dialogoHechizera1 , statsgetHelp, 4);
                agregarHijo(askHelp, hechiceraDialogoA);
    
                Nodo *hechiceraDialogoB= crearEvento(51, "Suplicar", "Rápidamente te das cuenta de que solo pedir ayuda no será suficiente. Te arrodilas enfrente de la hechicera para ofrecer tu lealtad y obediencia. Además, como muestra de buena fe, le empiezas a besar los pies, causando que el guerrero y la arquera rompan en carcajadas, avergonzando a la hechicera aún más.\n\n-Por favor, haré lo que quieras, solo necesito comer y saber adonde ir. No tengo nada conmigo, pero estoy a tu merced. Si fueras tan amable, bendiceme con tu bondad y misericordia, oh, hechicera piadosa. Si quieres puedo ser tu esclavo, pero por favor, so...- le ruegas a la hechicera, esperando que tu oferta sea suficiente para ganar su ayuda. Pero ella te interrumpe porque ya no puede aguantar la verguenza y simplemente retrocede mientras se tapa su cara sonrojada\nDespués de reirse, el guerrero mira los recursos y luego a sus compañeros antes de asentir. -Trato hecho. Te ayudaremos.- dice, y te pasa una espada extra que le sobraba también. Ganas 25 puntos de vida\n\nDespués de que te has recuperado un poco, Amari, la hechicera, te presenta a los otros miembros del grupo.\n\n-Yo soy Amari- dice la hechicera con una sonrisa. Soy una hechicera de los bosques del norte, siempre en busca de nuevos conocimientos y aventuras.\n-Thorne-, el guerrero, asiente con la cabeza. -Soy Thorne, de las tierras del oeste. He vagado por muchas regiones, protegiendo a los débiles y luchando contra la injusticia.-\nFinalmente, la arquera elfa, se adelanta y te mira con ojos escrutadores y evasivos.-Soy Lyra, del reino de los elfos en el este. Mi puntería es infalible, así que mas te vale no meterte en mi camino\n\nCon las presentaciones hechas, el grupo comienza a moverse, y decides seguirlos, mirando a los tres escéptico pero esperando que esta alianza te ayude en tu viaje. Y justo cuando pasan por la primera colina, su confianza es puesta a prueba, ya que son emboscados por una banda de 4 duendes que aparece de unos arbustos rodeandolos.\nDos de estos enemigos atacan a la elfa, quién iba primero, forzando a la hechicera y el guerrero defenderla, pero le dan la espalda a los otros dos duendes, los cuales al principio te ignoran para aprovecharla oportunidad de atacarlos. Y en ese momento, te encontraste en frente de dos opciones.\n", statsgetHelp, 4);
                agregarHijo(askHelp, hechiceraDialogoB);

                Nodo *hechiceraDialogoC= crearEvento(52, "Inventar una historia (CH 3)", "Decides que la mejor manera de ganarte su confianza es contar una historia convincente sobre cómo llegaste a estar en esta situación. Inventas un relato sobre ser atacado por bandidos y perder todo, incluyendo a tus compañeros.\n\nLa hechicera te observa atentamente, tratando de decidir si creer en tu historia. Después de un momento, asiente ligeramente.\n-Está bien, te ayudaremos,- dice con cierta cautela, -pero no intentes nada extraño.-\nSus compañeros son aún más sospechosos, pero el guerrero igual te pasa una espada que tenía de sobra, aunque lo hace de mala gan, Ganas 25 puntos de vida\n\nDespués de que te has recuperado un poco, Amari, la hechicera, te presenta a los otros miembros del grupo.\n\n-Yo soy Amari- dice la hechicera con una sonrisa. Soy una hechicera de los bosques del norte, siempre en busca de nuevos conocimientos y aventuras.\n-Thorne-, el guerrero, asiente con la cabeza. -Soy Thorne, de las tierras del oeste. He vagado por muchas regiones, protegiendo a los débiles y luchando contra la injusticia.-\nFinalmente, la arquera elfa, se adelanta y te mira con ojos escrutadores y evasivos.-Soy Lyra, del reino de los elfos en el este. Mi puntería es infalible, así que mas te vale no meterte en mi camino\n\nCon las presentaciones hechas, el grupo comienza a moverse, y decides seguirlos, mirando a los tres escéptico pero esperando que esta alianza te ayude en tu viaje. Y justo cuando pasan por la primera colina, su confianza es puesta a prueba, ya que son emboscados por una banda de 4 duendes que aparece de unos arbustos rodeandolos.\nDos de estos enemigos atacan a la elfa, quién iba primero, forzando a la hechicera y el guerrero defenderla, pero le dan la espalda a los otros dos duendes, los cuales al principio te ignoran para aprovecharla oportunidad de atacarlos. Y en ese momento, te encontraste en frente de dos opciones.\n", statsgetHelp, 4);
                hechiceraDialogoC->condiciones = condicionCarisma1;
                agregarHijo(askHelp, hechiceraDialogoC);

                     // Nodo 5B.B.A.A.A: Abandonar grupo
                        Nodo *abandonarGrupo = crearEvento(82, "Abandonar al grupo y huir solo", 
                            "Viendo la emboscada inminente, decides que es mejor salvar tu propia piel. Sin decir una palabra, te alejas del grupo y corres en dirección opuesta a la de los duendes, buscando una salida segura del valle, la cual es seguir yendo al este. Escapas momentáneamente de la emboscada, pero te sientes culpable por dejar a Amari, Thorne y Lyra atrás. Aún puedes escuchar su sufrimiento, e incluso cuando llaman por ayuda, no miras atrás y sigues adelante, ignorando el resto del mundo para encontrar un lugar segurp. Después de un rato, estás lo suficientemente lejos para descansar, te das las vuelta y no puees ver nada más allá de las colinas, pero si encuentras una oportunidad de continuar tu viaje en solitario.\n\nEl fin, womp womp.\n", statsDefault, 5);
                        agregarHijo(hechiceraDialogoA, abandonarGrupo);
                        agregarHijo(hechiceraDialogoB, abandonarGrupo);
                        agregarHijo(hechiceraDialogoC, abandonarGrupo);
    
                    // Nodo 5B.B.A.A.A: Luchar con grupo (Batalla)
                        Player statsHelpGroup = {0};
                        statsHelpGroup.strenght += 1;
                        Nodo *lucharConGrupo = crearEventoMinijuego(83, "Permanecer y luchar junto al grupo", 
                            "Decides quedarte junto a Amari, Thorne y Lyra para enfrentar a los duendes. El grupo se prepara para la batalla, usando sus habilidades y trabajo en equipo para contrarrestar la emboscada. Aunque estás nervioso, te sientes obligado a pelear también, por lo que persigues a un duende que estaba a punto de atrapar a Lyra mientras ella apuntaba. Solo con tacklear al duende y salvar a la arquera te sientes más fuerte. Ganas un punto de fuerza, pero te das cuenta que tu contrincante probablemente es el más fuerte de la banda; es un duende mamadisimo casi de tu mismo tamaño, pero aún así no dudas en enfrentarlo por el bien del grupo\n", statsHelpGroup, enemigoGoblin2);
                        agregarHijo(hechiceraDialogoA, lucharConGrupo); // Los otros dialogos tienen acceso a traves de hermanos

                        

            // Nodo 3B.B.B: Seguir al grupo
            Nodo *followSquad = crearEvento(21, "Seguir al grupo a distancia", "Sin la certeza de poder confiar en el grupo, decides mantenerte a una distancia segura y seguirlos bordeando el bosque hasta que llegan a una colina elevada que no te deja verlos. Te ves forzado a acercarte y silenciosamente vas a la colina. El grupo parece haberse detenido en la base de la colina, donde empieza un camino con arbustos grandes que crean sospecha.\n\n Tomas este momento para ver al grupo más de cerca e identificas a los tres individuos; una hechicera debido al sombrero que lleva puesto y el bastón mágico que tiene en la mano, un guerrero de pelo corto y marrón con un hacha más grande que su cuerpo entero, y una arquera rubia con orejas largas, por lo que la identificas como una elfa.\n\n Esta última tiene la iniciativa de revisar los arbustos, pero antes de poder inspeccionarlos, cuatro duendes se revelan de los arbustos, saltando al grupo y pillándolos en una posición vulnerable. Dos de estos enemigos atacan a la elfa, forzando a la hechicera y el guerrero defenderla, pero le dan la espalda a los otros dos duendes, los cuales aprovechan la oportunidad para atacar.\n Tu miras el combate por unos segundos y te das cuenta que los duendes no te han visto, por lo que puedes ir a distraerlos por un rato o quedarte viendo y esperar a que acabe la pelea.\n", statsDefault, 3);
            agregarHijo(startRight_2, followSquad);
            agregarHijo(ignorarCampamento, followSquad);

    // Nodo 1C: Esperar por ayuda
    Player statsWait = {0};
    statsWait.health += -20; // Pierdes más salud al esperar
    Nodo *startWait = crearEvento(22, "Esperar por ayuda", "Nadie viene a tu rescate pero aun tienes esperanza. Pierdes 20 puntos de vida\n", statsWait, 1);
    agregarHijo(arbol->root, startWait);
    Nodo *startWaitMore = crearEvento(100, "Esperar más", "No sé que esperabas, pero nadie viene y mueres de hambre. Este es el fin de tu aventura y ni siquiera empezó.\n", statsDead, 2);
    agregarHijo(startWait, startWaitMore);
     
     
     Nodo* tutorial = crearEventoMinijuego(101, "Tutorial Batalla", "Te encuentras con un feroz enemigo y debes luchar, es hora de poner a prueba tus habilidades.", statsDefault, enemigoGoblin);
    agregarHijo(arbol->root, tutorial);
    
    Player statsFull = {0};
    statsWait.endurance += 1; // Pierdes más salud al esperar
    Nodo *continuarHistoria = crearEvento(102, "Continuar Historia", "Ahora que sabes luchar, comienza la historia de verdad\n", statsFull, 2);
    agregarHijo(tutorial, continuarHistoria);
    agregarHijo(continuarHistoria, arbol->root);



    

}

