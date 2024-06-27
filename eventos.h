#ifndef EVENTOS_H
#define EVENTOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tdas/list.h"
#include "tdas/stack.h"
#include "tdas/queue.h"
#include "tdas/extra.h"

void crearNodosHistoria(ArbolHistoria *arbol, Player jugador, List *inventario);

#endif /* EVENTOS_H */