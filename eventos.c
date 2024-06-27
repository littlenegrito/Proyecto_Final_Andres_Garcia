#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tdas/list.h"
#include "tdas/stack.h"
#include "tdas/queue.h"
#include "tdas/extra.h"


void crearNodosHistoria(ArbolHistoria *arbol, Player jugador, List *inventario) {
    Player statsDefault = {0};

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
                    Nodo *defendTrader = crearEvento(6, "Defender al vendedor", "Antes de que el ogro devore al vendedor, agarras una espada que este botó y decides defenderlo, poniéndote en frente del vendedor para absorber el primer golpe del ogro con la espada. El vendedor actúa rápido y le arroja una poción de daño que deja al ogro aturdido, y aprovechas a seguir atacando\n", statsDefault, 5);
                    agregarHijo(runStraight, defendTrader);

                    // Nodo 5A.A.A.A.B: Dejar al vendedor
                    Nodo *sacrificeTrader = crearEvento(7, "Dejar al vendedor", "A pesar de tener suministros a su disposición, arriesgar tu vida para salvar al vendedor no lo vale, especialmente al enfrentar un monstruo tan terrible, así que sigues corriendo. Intentas no mirar atrás mientras que el ogro se devora al vendedor, y apenas llegas a una bifurcación en un valle, evalúas los dos caminos a tus lados, y como los dos están vacíos, no piensas mucho antes de ir en una dirección.\n", statsDefault, 5);
                    agregarHijo(runStraight, sacrificeTrader);

                // Nodo 4A.A.A.B: Regresar al bosque
                Player statsForest = {0};
                statsForest.health -= 5; 
                Nodo *backForest = crearEvento(8, "Doblar y regresar al bosque", "Para despistar al ogro, decides dar un giro brusco y regresar al bosque. El monstruo te sigue pero logras correr más rápido, aunque en el intento de escapar, no prestas atención a tus pasos y te tropiezas con un tronco justo antes de caer bajo una colina. La caída no es tan severa, pero al llegar al suelo, te pegas en la cabeza con una roca y te quita 5 de vida.\n", statsForest, 4);
                agregarHijo(ogreRun, backForest);

            // Nodo 3A.A.B: Esconderse del ogro
            Nodo *ogreHide = crearEvento(9, "Esconderse en la cueva", "Para evitar el campo de visión del ogro, decides correr dentro de la cueva. El ogro es lento y le toma unos segundos antes de llegar a la entrada de la cueva. En pánico, te refugias en la oscuridad, manteniéndote en el suelo hasta que sientes el mango de una espada. Equipas la espada para defenderte pero esperas a que el ogro pase. Por suerte, no investigó la cueva, y luego de un rato, asomas la cabeza. El ogro ya no está, pero ves que en la posición que te encontrabas anteriormente, justo antes de la esquina de las rocas, el monstruo dejó un rastro de sangre que se dirige al bosque.\n", statsDefault, 3);
            agregarHijo(startLeft_1, ogreHide);

                // Nodo 4A.A.B.A: Seguir rastro de sangre
                Nodo *followBlood = crearEvento(10, "Seguir rastro de sangre", "Sigues el rastro de sangre dejado por el ogro hasta llegar a un claro en el bosque. La sangre se dispersa y no puedes seguir el rastro. Tienes dos opciones: continuar hacia el bosque profundo o regresar y explorar otros caminos.\n", statsDefault, 4);
                agregarHijo(ogreHide, followBlood);

                // Nodo 4A.A.B.B: Seguir camino hasta llegar a bifurcación
                Nodo *crossroads = crearEvento(11, "Seguir el camino por el río", "Cualquier cosa que haya pasado con el ogro, seguramente es peligroso, así que evitas el peligro al seguir recto. El río te lleva a un valle con un camino de tierra que parece ser de una ruta transitada. Tienes muchas direcciones para ir, pero como no ves mucho más que colinas en frente, revisas tu izquierda y derecha. A la izquierda, el camino con el río sigue a una zona más oculta pero que parece estar vacía. En cambio, a la derecha, ves dos personas corriendo por sus vidas. Por la distancia, no logras reconocer que les está pasando, pero después de unos segundos, ves una horda de duendes persiguiéndolos. Los dos individuos en pánico se están acercando a tu posición, pero es poco probable que no sobrevivan ya que los duendes están a punto de acorralarlos.\n", statsDefault, 4);
                agregarHijo(ogreHide, crossroads);

                    // Nodo 5A.A.B.B.A: Defender contra duendes (BATALLA)
                    Nodo *interveneAttack = crearEvento(12, "Ayudar a defender contra duendes", "Como tienes una espada y aún no has sido visto por los duendes, decides ayudar un poco, siendo cauteloso para atacar a los duendes más aislados y distraer a la horda.\n", statsDefault, 5);
                    agregarHijo(crossroads, interveneAttack);

                    // Nodo 5A.A.B.B.B: Ir al otro lado
                    Nodo *avoidConflict = crearEvento(13, "Evitar conflicto yendo a la otra dirección", "No quieres ser parte de la pelea, así que te das la vuelta y vas en el sentido contrario lo antes posible, tratando de no mirar atrás para no arriesgar ser visto. Puedes escuchar los individuos gritando por ayuda mientras son masacrados por los duendes, pero esto solo te motiva a correr más rápido hasta llegar a una distancia segura.\n\nCuando ya no puedes escuchar la pelea, te das la vuelta a revisar tu alrededor y ves que el río conecta a una laguna adelante. No logras ver qué hay detrás de unos árboles rodeando el cuerpo de agua, así que te acercas a la laguna. Asomándote detrás de un árbol, descubres una cabaña con una pequeña granja de pollos en el patio, pero notas que las luces están prendidas, así que te tomas un segundo para pensar en qué hacer.\n", statsDefault, 5);
                    agregarHijo(crossroads, avoidConflict);

                        // Nodo 6A.A.B.B.B: Tocar la puerta
                        Nodo *tocarPuerta = crearEvento(28, "Tocar la puerta y presentarte", "Tocas la puerta\n", statsDefault, 6);
                        agregarHijo(avoidConflict, tocarPuerta);

                        // Nodo 6A.A.B.B.B: Robar gallina
                        Nodo *robarGallina = crearEvento(29, "Robar una gallina para comer", "Robas la gallina\n", statsDefault, 6);
                        agregarHijo(avoidConflict, robarGallina);

        // Nodo 2A.B: Explorar Cueva (BATALLA)
        Player statsLeft_2 = {0};
        statsLeft_2.gold += 15; 
        Nodo *startLeft_2 = crearEvento(14, "Explorar Cueva", "Cruzas el río para explorar la cueva. Encuentras una espada para defenderte en la entrada al lado de un esqueleto. Tomas la espada y sigues explorando hasta encontrar una bolsa con 50 de oro. Celebras por un segundo hasta que escuchas un gruñido afuera de la cueva. Sales a explorar y te encuentras con un ogro esperándote, no tienes otra opción que luchar.\n", statsLeft_2, 2);
        agregarHijo(startLeft, startLeft_2);

    // Nodo 1B: Ir a la derecha
    Nodo *startRight = crearEvento(15, "Ir a la derecha", "Sigues un camino y te mantienes en el bosque hasta que te encuentras con unas bayas misteriosas y un camino despejado adelante.\n", statsDefault, 1);
    agregarHijo(arbol->root, startRight);

        // Nodo 2B.A: Comer bayas
        Player statsRight_1 = {0};
        statsRight_1.endurance += 1; 
        Nodo *startRight_1 = crearEvento(16, "Comer bayas", "Agarras un puñado de bayas para saciar el hambre. Tiene un sabor peculiar, empieza dulce y luego amargo. No puedes resistir el sabor y te atoras. Finalmente, aguantas y tragas las bayas, pero cuando llegan a tu estómago, sientes un dolor agudo que te deja aturdido. Ganas 1 de resistencia.\n", statsRight_1, 2);
        agregarHijo(startRight, startRight_1);

            // Nodo 3B.A.A: Descansar (BATALLA)
            Player statsRest = {0};
            statsRest.health += 5; 
            Nodo *berriesRest = crearEvento(17, "Descansar y esperar ayuda", "Descansas y recuperas 5 de salud, pero pierdes tiempo. Te despiertas en el medio de la noche y eres emboscado por un oso salvaje y debes defenderte.\n", statsRest, 3);
            agregarHijo(startRight_1, berriesRest);

            // Nodo 3B.A.B: Seguir caminando
            Nodo *ignoreStomachAche = crearEvento(18, "Ignorar malestar y seguir", "Decides ignorar el malestar y continúas con tu camino. Te das cuenta que te desviaste y no seguiste recto, pero en vez de regresar, te acercas un poco más a una fuente de sonido cercana. Sigues el camino equivocado hasta encontrar un campamento que parece estar desolado.\n", statsDefault, 3);
            agregarHijo(startRight_1, ignoreStomachAche);

        // Nodo 2B.B: Ignorar bayas
        Nodo *startRight_2 = crearEvento(19, "Ignorar bayas", "Decides no comer las bayas y a pesar de tu hambre, sigues el camino adelante hasta que sales del bosque. Te encuentras con un valle grande y un camino adyacente a un río a tu izquierda, pero mientras estás evaluando tu alrededor, ves a un grupo de tres personas yéndose por el camino, alejándose cada vez más. Como no hay nadie más presente, te tomas tu tiempo para decidir qué hacer.\n", statsDefault, 2);
        agregarHijo(startRight, startRight_2);

            // Nodo 3B.B.A: Pedir ayuda al grupo
            Nodo *askHelp = crearEvento(20, "Pedir ayuda", "Antes de que sea muy tarde, corres detrás del grupo, gritándoles por ayuda hasta que se dan la vuelta. Al principio, adoptan la defensa y sacan sus armas, pero después de ver que estás desarmado, te dejan acercarte antes de revisar tu condición.\n\nEl grupo consiste de una hechicera por el sombrero y el bastón mágico que tiene en la mano, un guerrero de pelo corto y marrón con un hacha más grande que su cuerpo entero, y una arquera rubia con orejas largas, por lo que la identificas como una elfa. Solo la hechicera se acerca para ofrecer ayuda, pero antes de pasarte unas vendas y agua, te mira directo a los ojos y pregunta\n-Hey, detente ahí. Dinos quién eres y qué es lo que buscas o tendremos que defendernos-\n", statsDefault, 3);
            agregarHijo(startRight_2, askHelp);

            // Nodo 3B.B.B: Seguir al grupo
            Nodo *followSquad = crearEvento(21, "Seguir al grupo a distancia", "Sin la certeza de poder confiar en el grupo, decides mantenerte a una distancia segura y seguirlos bordeando el bosque hasta que llegan a una colina elevada que no te deja verlos. Te ves forzado a acercarte y silenciosamente vas a la colina. El grupo parece haberse detenido en la base de la colina, donde empieza un camino con arbustos grandes que crean sospecha.\n\n Tomas este momento para ver al grupo más de cerca e identificas a los tres individuos; una hechicera debido al sombrero que lleva puesto y el bastón mágico que tiene en la mano, un guerrero de pelo corto y marrón con un hacha más grande que su cuerpo entero, y una arquera rubia con orejas largas, por lo que la identificas como una elfa.\n\n Esta última tiene la iniciativa de revisar los arbustos, pero antes de poder inspeccionarlos, cuatro duendes se revelan de los arbustos, saltando al grupo y pillándolos en una posición vulnerable. Dos de estos enemigos atacan a la elfa, forzando a la hechicera y el guerrero defenderla, pero le dan la espalda a los otros dos duendes, los cuales aprovechan la oportunidad para atacar.\n Tu miras el combate por unos segundos y te das cuenta que los duendes no te han visto, por lo que puedes ir a distraerlos por un rato o quedarte viendo y esperar a que acabe la pelea.\n", statsDefault, 3);
            agregarHijo(startRight_2, followSquad);

    // Nodo 1C: Esperar por ayuda
    Player statsWait = {0};
    statsWait.health -= 100; // Pierdes más salud al esperar
    Nodo *startWait = crearEvento(22, "Esperar por ayuda", "Nadie viene a tu rescate y mueres de hambre.\n", statsWait, 1);
    agregarHijo(arbol->root, startWait);

    Nodo *TEST = crearEvento(76, "TEST 1", "Esta es una prueba\n", statsDefault, 1);
    agregarHijo(arbol->root, TEST);

    //Condicion *condicionVelocidad = crearCondicion("agility", 3);
    //ogreRun->condiciones = condicionVelocidad;

}