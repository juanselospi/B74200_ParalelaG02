#include <iostream>
#include <omp.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
using namespace std;

/*
================================================================================
 SIMULACIÓN DEL PROBLEMA DEL CANOPY (MONOS + CUERDAS) USANDO OPENMP
 VERSIÓN CON SOLUCIÓN A: 
 → CUANDO UN MONO COMPLETA TURF CRUCES, TERMINA SU HILO (return).
 Esto evita loops infinitos y garantiza que la simulación finaliza sola.

 Basado en las reglas:
 - Hay varias cuerdas (ropes)
 - Cada cuerda tiene:
       • Dirección actual (LEFT o RIGHT)
       • Máximo de monos por turno (TURNS)
 - Los monos se aproximan aleatoriamente y cruzan solo si:
       • La cuerda va en su dirección
       • No se ha excedido TURNS
 - Cuando una cuerda llega a TURNS y queda vacía → cambia de dirección.
================================================================================
*/

//------------------------------------------------------
// Parámetros globales de simulación
//------------------------------------------------------
#define TROOP 10          // cantidad de monos
#define ROPE_COUNT 2      // cuántas cuerdas hay
#define TURF 5            // cuántas veces cruza cada mono
#define TURNS 3           // máximo de cruces por turno y dirección

//------------------------------------------------------
// Dirección del mono
//------------------------------------------------------
enum Direction { LEFT, RIGHT };

//------------------------------------------------------
// Datos de la cuerda (estructura compartida por todos los hilos)
//------------------------------------------------------
struct RopeData {
    omp_lock_t lock;      // exclusión mutua para esta cuerda
    Direction dir;        // dirección actual de cruce
    int onRope;           // cuántos monos cruzan en este momento
    int crossedThisTurn;  // cuántos han cruzado en este turno
    int ropeId;           // ID solo para impresión
};

//------------------------------------------------------
// Intentar entrar a la cuerda
//------------------------------------------------------
bool tryEnter(RopeData &rope, int monkeyId, Direction dir) {
    bool canEnter = false;

    omp_set_lock(&rope.lock);

    // Si no hay monos sobre la cuerda, esta adopta la dirección del que llega
    if (rope.onRope == 0) {
        rope.dir = dir;
    }

    // Solo puede entrar si la dirección coincide y no excede TURNS
    if (rope.dir == dir && rope.crossedThisTurn < TURNS) {
        rope.onRope++;
        rope.crossedThisTurn++;

        cout << "Monkey " << monkeyId
             << " ENTRA a la rope " << rope.ropeId
             << " en dirección " 
             << ((dir == LEFT) ? "LEFT -> RIGHT" : "RIGHT -> LEFT")
             << " (onRope=" << rope.onRope
             << ", crossedThisTurn=" << rope.crossedThisTurn
             << ")\n";

        canEnter = true;
    }

    omp_unset_lock(&rope.lock);
    return canEnter;
}

//------------------------------------------------------
// El mono cruza la cuerda
//------------------------------------------------------
void cross(RopeData &rope, int monkeyId, Direction dir) {
    cout << "Monkey " << monkeyId 
         << " está CRUZANDO la rope " << rope.ropeId
         << " hacia "
         << ((dir == LEFT) ? "LEFT -> RIGHT" : "RIGHT -> LEFT")
         << "\n";

    // Simular tiempo de cruce
    usleep(100000 + (rand() & 0xFFFF));
}

//------------------------------------------------------
// Salir de la cuerda y manejar posible cambio de dirección
//------------------------------------------------------
void leave(RopeData &rope, int monkeyId, Direction dir) {

    omp_set_lock(&rope.lock);

    rope.onRope--;

    cout << "Monkey " << monkeyId 
         << " SALE de la rope " << rope.ropeId
         << " (onRope=" << rope.onRope << ", dir="
         << ((dir == LEFT) ? "LEFT -> RIGHT" : "RIGHT -> LEFT")
         << ")\n";

    // Si la cuerda quedó vacía…
    if (rope.onRope == 0) {

        if (rope.crossedThisTurn >= TURNS) {
            cout << ">> Rope " << rope.ropeId 
                 << " completó un turno de " << TURNS 
                 << " cruces. Esperando posible cambio de dirección...\n";

            // Cambiar dirección
            rope.dir = (rope.dir == LEFT) ? RIGHT : LEFT;

            rope.crossedThisTurn = 0;

            cout << ">> Rope " << rope.ropeId 
                 << " cambia dirección a "
                 << ((rope.dir == LEFT) ? "LEFT -> RIGHT" : "RIGHT -> LEFT")
                 << "\n";
        }
    }

    omp_unset_lock(&rope.lock);
}

//------------------------------------------------------
// Comportamiento completo del mono
//------------------------------------------------------
void monkeyBehavior(int monkeyId, RopeData ropes[]) {
    Direction dir = (rand() % 2 == 0) ? LEFT : RIGHT;

    for (int stroll = 0; stroll < TURF; stroll++) {

        // Monito se distrae antes de intentar cruzar
        usleep(rand() & 0x1FFF);

        int ropeIndex = rand() % ROPE_COUNT;
        RopeData &rope = ropes[ropeIndex];

        cout << "Monkey " << monkeyId
             << " se acerca a la rope " << rope.ropeId
             << " desde el lado " 
             << ((dir == LEFT) ? "LEFT" : "RIGHT")
             << "\n";

        // Intentar entrar hasta lograrlo
        while (!tryEnter(rope, monkeyId, dir)) {
            usleep(2000);
        }

        // Cruzar
        cross(rope, monkeyId, dir);

        // Salir de la cuerda
        leave(rope, monkeyId, dir);

        // Monito cambia de lado
        dir = (dir == LEFT) ? RIGHT : LEFT;
    }

    // *** SOLUCIÓN A ***
    // Cuando el mono termina sus TURF cruces:
    cout << "Monkey " << monkeyId << " terminó todos sus cruces.\n";
    return; // <-- FINALIZA ESTE HILO (no intenta cruzar más)
}

//------------------------------------------------------
// MAIN
//------------------------------------------------------
int main() {
    srand(time(NULL));

    cout << "=== Simulación de monos en el canopy con OpenMP ===\n";
    cout << "TROOP (monos)   = " << TROOP << "\n";
    cout << "ROPE_COUNT      = " << ROPE_COUNT << "\n";
    cout << "TURF (cruces)   = " << TURF << " por mono\n";
    cout << "TURNS (máx. monos por turno y dirección) = " << TURNS << "\n\n";

    // Inicializar cuerdas
    RopeData ropes[ROPE_COUNT];
    for (int i = 0; i < ROPE_COUNT; i++) {
        omp_init_lock(&ropes[i].lock);
        ropes[i].dir = LEFT;
        ropes[i].onRope = 0;
        ropes[i].crossedThisTurn = 0;
        ropes[i].ropeId = i + 1;
    }

    // Región paralela: cada hilo es un mono
    #pragma omp parallel num_threads(TROOP)
    {
        int id = omp_get_thread_num();
        monkeyBehavior(id, ropes);
    }

    cout << "\n=== Simulación finalizada correctamente ===\n";
    return 0;
}
