# Proyecto de Programación Paralela y Concurrente

Este proyecto contiene la implementación de dos problemas clásicos de programación paralela y concurrente utilizando procesos y sincronización en **C++**.

---

## Problemas Implementados

### a) Problema de los Monos
**Descripción:**  
Simulación del problema donde **N monos** deben cruzar un barranco usando una cuerda con capacidad para **K monos**.  
Los monos no pueden retroceder y deben coordinarse para evitar caídas. Después de que **R monos** cruzan en una dirección, la cuerda debe cambiar de dirección si hay monos esperando en el lado opuesto.

**Características:**
- Cada mono es un proceso independiente (`fork`)
- Dirección de cruce asignada aleatoriamente
- Soporte para una y dos cuerdas
- Sincronización para prevenir caídas y rompimiento de la cuerda

---

### b) Juego de la "Papa" Caliente (Pringles)
**Descripción:**  
Implementación del juego donde **N procesos** forman un anillo y se pasan una *"papa caliente"* (token).  
Cada proceso aplica las reglas de **Collatz** al valor de la papa, y si el resultado es `1`, el proceso sale del juego.  
Incluye un proceso **malicioso** que envía mensajes falsos para confundir a los participantes.

**Características:**
- Anillo de procesos con comunicación mediante buzones
- Aplicación de reglas de Collatz para determinar explosiones
- Dirección de giro configurable ( horario / antihorario )
- Proceso malicioso que envía mensajes falsos
- Detección y filtrado de mensajes inválidos

---

## Requisitos del Sistema
- Compilador de **C++** con soporte para `semaforos` y `forks`
- **Make**
- Sistema operativo **Linux**
- Bibliotecas estándar de C++

---

## Compilación y Ejecución

### a) Problema de los Monos

- Compilar: make
- Ejecutar: ./bin/Monkeys.out 

**OPCIONAL:** 
./bin/Monkeys.out [N] `donde N es el número de monos como parámetro opcional`

---

### b) Problema de la Papa Caliente (Pringles)

- Compilar: make bin/Pringles.out
- Ejecutar: ./bin/Pringles.out

**OPCIONAL:** 
./bin/Pringles.out [n] [v] [dir] 

`n`: Número de participantes en la ronda
`v`: Valor inicial de la papa
`dir`: 1 para horario, -1 para antihorario

---

*Limpieza*
- Para limpiar los archivos compilados: make clean

## Ejemplos de salida

### a) Monkeys.out

- Ejemplo de salida para parametro opcional de `10 monos`:

Monkey 9 picked rope 2
Monkey 9 is crossing the rope [2] LEFT -> RIGHT
Monkey 4 picked rope 2
Monkey 4 is crossing the rope [2] LEFT -> RIGHT
Monkey 8 picked rope 2
Monkey 8 is crossing the rope [2] LEFT -> RIGHT
Monkey 7 picked rope 1
The rope [1] changes direction to RIGHT -> LEFT
Monkey 7 is crossing the rope [1] RIGHT -> LEFT
Monkey 6 picked rope 1
Monkey 3 picked rope 1
Monkey 3 is crossing the rope [1] RIGHT -> LEFT
Monkey 0 picked rope 2
Monkey 0 is crossing the rope [2] LEFT -> RIGHT
Monkey 5 picked rope 2
Monkey 1 picked rope 1
Monkey 2 picked rope 2
Monkey 2 is crossing the rope [2] LEFT -> RIGHT
Monkey 9 picked rope 1
Monkey 9 is crossing the rope [1] RIGHT -> LEFT
Monkey 7 picked rope 1
Monkey 0 picked rope 2
The rope [2] changes direction to RIGHT -> LEFT
Monkey 0 is crossing the rope [2] RIGHT -> LEFT
Monkey 5 is crossing the rope [2] RIGHT -> LEFT
Monkey 4 picked rope 2
Monkey 4 is crossing the rope [2] RIGHT -> LEFT
Monkey 8 picked rope 1
Monkey 8 is crossing the rope [1] RIGHT -> LEFT
Monkey 2 picked rope 2
Monkey 2 is crossing the rope [2] RIGHT -> LEFT
Monkey 3 picked rope 1
Monkey 9 picked rope 1
The rope [1] changes direction to LEFT -> RIGHT
Monkey 9 is crossing the rope [1] LEFT -> RIGHT
Monkey 6 is crossing the rope [1] LEFT -> RIGHT
Monkey 1 is crossing the rope [1] LEFT -> RIGHT
Monkey 3 is crossing the rope [1] LEFT -> RIGHT
Monkey 7 is crossing the rope [1] LEFT -> RIGHT
Monkey 0 picked rope 2
Monkey 5 picked rope 1
Monkey 4 picked rope 1
The rope [2] changes direction to LEFT -> RIGHT
Monkey 0 is crossing the rope [2] LEFT -> RIGHT
Monkey 8 picked rope 1
Monkey 2 picked rope 2
Monkey 2 is crossing the rope [2] LEFT -> RIGHT
Monkey 4 is crossing the rope [1] LEFT -> RIGHT
Monkey 8 is crossing the rope [1] LEFT -> RIGHT
Monkey 7 picked rope 1
Monkey 5 is crossing the rope [1] LEFT -> RIGHT
Monkey 3 picked rope 1
Monkey 9 picked rope 2
Monkey 1 picked rope 2
The rope [2] changes direction to RIGHT -> LEFT
Monkey 9 is crossing the rope [2] RIGHT -> LEFT
Monkey 1 is crossing the rope [2] RIGHT -> LEFT
Monkey 0 picked rope 2
Monkey 0 is crossing the rope [2] RIGHT -> LEFT
Monkey 6 picked rope 2
Monkey 6 is crossing the rope [2] RIGHT -> LEFT
Monkey 2 picked rope 1
Monkey 4 picked rope 1
The rope [1] changes direction to RIGHT -> LEFT
Monkey 3 is crossing the rope [1] RIGHT -> LEFT
Monkey 2 is crossing the rope [1] RIGHT -> LEFT
Monkey 7 is crossing the rope [1] RIGHT -> LEFT
Monkey 4 is crossing the rope [1] RIGHT -> LEFT
Monkey 5 picked rope 1
Monkey 5 is crossing the rope [1] RIGHT -> LEFT
Monkey 8 picked rope 1
Monkey 1 picked rope 2
The rope [2] changes direction to LEFT -> RIGHT
Monkey 1 is crossing the rope [2] LEFT -> RIGHT
Monkey 8 is crossing the rope [1] RIGHT -> LEFT
Monkey 0 picked rope 2
Monkey 0 is crossing the rope [2] LEFT -> RIGHT
Monkey 9 picked rope 2
Monkey 9 is crossing the rope [2] LEFT -> RIGHT
Monkey 6 picked rope 2
Monkey 6 is crossing the rope [2] LEFT -> RIGHT
Monkey 5 picked rope 1
Monkey 2 picked rope 1
Monkey 3 picked rope 1
Monkey 4 picked rope 1
Monkey 7 picked rope 2
Monkey 7 is crossing the rope [2] LEFT -> RIGHT
The rope [1] changes direction to LEFT -> RIGHT
Monkey 4 is crossing the rope [1] LEFT -> RIGHT
Monkey 2 is crossing the rope [1] LEFT -> RIGHT
Monkey 3 is crossing the rope [1] LEFT -> RIGHT
Monkey 5 is crossing the rope [1] LEFT -> RIGHT
Monkey 1 picked rope 1
Monkey 8 picked rope 1
Monkey 8 is crossing the rope [1] LEFT -> RIGHT
Monkey 0 picked rope 2
Monkey 9 picked rope 2
Monkey 6 picked rope 2
The rope [2] changes direction to RIGHT -> LEFT
Monkey 6 is crossing the rope [2] RIGHT -> LEFT
Monkey 0 is crossing the rope [2] RIGHT -> LEFT
Monkey 9 is crossing the rope [2] RIGHT -> LEFT
Monkey 7 picked rope 2
Monkey 7 is crossing the rope [2] RIGHT -> LEFT
Monkey 2 picked rope 2
Monkey 2 is crossing the rope [2] RIGHT -> LEFT
The rope [1] changes direction to RIGHT -> LEFT
Monkey 1 is crossing the rope [1] RIGHT -> LEFT
Monkey 5 picked rope 1
Monkey 5 is crossing the rope [1] RIGHT -> LEFT
Monkey 4 picked rope 2
Monkey 8 picked rope 1
Monkey 8 is crossing the rope [1] RIGHT -> LEFT
Monkey 3 picked rope 1
Monkey 3 is crossing the rope [1] RIGHT -> LEFT
Monkey 4 is crossing the rope [2] RIGHT -> LEFT
Monkey 0 picked rope 2
Monkey 7 picked rope 1
Monkey 6 picked rope 2
Monkey 1 picked rope 1
Monkey 9 picked rope 2
Monkey 2 picked rope 1
Monkey 5 picked rope 2
Monkey 8 picked rope 1
The rope [1] changes direction to LEFT -> RIGHT
Monkey 7 is crossing the rope [1] LEFT -> RIGHT
Monkey 1 is crossing the rope [1] LEFT -> RIGHT
Monkey 8 is crossing the rope [1] LEFT -> RIGHT
Monkey 2 is crossing the rope [1] LEFT -> RIGHT
The rope [2] changes direction to LEFT -> RIGHT
Monkey 5 is crossing the rope [2] LEFT -> RIGHT
Monkey 9 is crossing the rope [2] LEFT -> RIGHT
Monkey 6 is crossing the rope [2] LEFT -> RIGHT
Monkey 0 is crossing the rope [2] LEFT -> RIGHT
Monkey 3 picked rope 2
Monkey 3 is crossing the rope [2] LEFT -> RIGHT
Monkey 4 picked rope 1
Monkey 4 is crossing the rope [1] LEFT -> RIGHT
Monkey 2 picked rope 2
Monkey 7 picked rope 2
Monkey 8 picked rope 2
Monkey 9 picked rope 2
Monkey 1 picked rope 2
Monkey 3 picked rope 2
The rope [2] changes direction to RIGHT -> LEFT
Monkey 8 is crossing the rope [2] RIGHT -> LEFT
Monkey 2 is crossing the rope [2] RIGHT -> LEFT
Monkey 7 is crossing the rope [2] RIGHT -> LEFT
Monkey 3 is crossing the rope [2] RIGHT -> LEFT
Monkey 1 is crossing the rope [2] RIGHT -> LEFT
Monkey 5 picked rope 1
The rope [1] changes direction to RIGHT -> LEFT
Monkey 5 is crossing the rope [1] RIGHT -> LEFT
Monkey 6 picked rope 1
Monkey 6 is crossing the rope [1] RIGHT -> LEFT
Monkey 4 picked rope 1
Monkey 4 is crossing the rope [1] RIGHT -> LEFT
Monkey 0 picked rope 1
Monkey 0 is crossing the rope [1] RIGHT -> LEFT
Monkey 9 is crossing the rope [2] RIGHT -> LEFT
Monkey 7 picked rope 2
Monkey 5 picked rope 2
Monkey 2 picked rope 2
Monkey 3 picked rope 2
Monkey 1 picked rope 2
Monkey 0 picked rope 1
The rope [1] changes direction to LEFT -> RIGHT
Monkey 0 is crossing the rope [1] LEFT -> RIGHT
Monkey 4 picked rope 2
Monkey 6 picked rope 1
Monkey 6 is crossing the rope [1] LEFT -> RIGHT
Monkey 8 picked rope 2
The rope [2] changes direction to LEFT -> RIGHT
Monkey 5 is crossing the rope [2] LEFT -> RIGHT
Monkey 7 is crossing the rope [2] LEFT -> RIGHT
Monkey 4 is crossing the rope [2] LEFT -> RIGHT
Monkey 2 is crossing the rope [2] LEFT -> RIGHT
Monkey 8 is crossing the rope [2] LEFT -> RIGHT
Monkey 9 picked rope 2
Monkey 0 picked rope 2
The rope [1] changes direction to RIGHT -> LEFT
Monkey 9 is crossing the rope [2] LEFT -> RIGHT
Monkey 3 is crossing the rope [2] LEFT -> RIGHT
Monkey 6 picked rope 2
Monkey 5 picked rope 1
Monkey 5 is crossing the rope [1] RIGHT -> LEFT
Monkey 1 is crossing the rope [2] LEFT -> RIGHT
Monkey 7 picked rope 2
Monkey 4 picked rope 2
Monkey 2 picked rope 1
Monkey 2 is crossing the rope [1] RIGHT -> LEFT
Monkey 8 picked rope 1
Monkey 8 is crossing the rope [1] RIGHT -> LEFT
Monkey 9 picked rope 1
Monkey 9 is crossing the rope [1] RIGHT -> LEFT
Monkey 3 picked rope 1
Monkey 3 is crossing the rope [1] RIGHT -> LEFT
The rope [2] changes direction to RIGHT -> LEFT
Monkey 6 is crossing the rope [2] RIGHT -> LEFT
Monkey 7 is crossing the rope [2] RIGHT -> LEFT
Monkey 0 is crossing the rope [2] RIGHT -> LEFT
Monkey 4 is crossing the rope [2] RIGHT -> LEFT
Monkey 1 picked rope 2
Monkey 1 is crossing the rope [2] RIGHT -> LEFT
Monkey 5 picked rope 2
Monkey 3 picked rope 2
The rope [1] changes direction to LEFT -> RIGHT
The rope [2] changes direction to LEFT -> RIGHT
Monkey 5 is crossing the rope [2] LEFT -> RIGHT
Monkey 3 is crossing the rope [2] LEFT -> RIGHT
Monkey 6 picked rope 1
Monkey 6 is crossing the rope [1] LEFT -> RIGHT
Monkey 7 picked rope 1
Monkey 7 is crossing the rope [1] LEFT -> RIGHT
Monkey 1 picked rope 1
Monkey 1 is crossing the rope [1] LEFT -> RIGHT
Monkey 6 picked rope 2
The rope [2] changes direction to RIGHT -> LEFT
Monkey 6 is crossing the rope [2] RIGHT -> LEFT
Monkey 1 picked rope 1
The rope [1] changes direction to RIGHT -> LEFT
Monkey 1 is crossing the rope [1] RIGHT -> LEFT

### b) Pringles.out

- Ejemplo de salida para parametro opcional de `giro antihorario`:

[GENUINE] Player 4 got explosive [19] from: -1
 ...generating Collatz!
[GENUINE] Player 3 got explosive [58] from: 4
 ...generating Collatz!
[INVADER] Sent fake message to player 4 with value 67
[INVADER] Faker 4 sent fake [67] from origin -99 to [GENUINE] Player 4
[GENUINE] Player 2 got explosive [29] from: 3
 ...generating Collatz!
[GENUINE] Player 1 got explosive [88] from: 2
 ...generating Collatz!
[INVADER] Sent fake message to player 0 with value 92
[INVADER] Faker 0 sent fake [92] from origin -99 to [GENUINE] Player 0
[GENUINE] Player 0 got explosive [44] from: 1
 ...generating Collatz!
[GENUINE] Player 4 got explosive [22] from: 0
 ...generating Collatz!
[INVADER] Sent fake message to player 3 with value 99
[INVADER] Faker 3 sent fake [99] from origin -99 to [GENUINE] Player 3
[GENUINE] Player 3 got explosive [11] from: 4
 ...generating Collatz!
[GENUINE] Player 2 got explosive [34] from: 3
 ...generating Collatz!
[INVADER] Sent fake message to player 0 with value 45
[INVADER] Faker 0 sent fake [45] from origin -99 to [GENUINE] Player 0
[GENUINE] Player 1 got explosive [17] from: 2
 ...generating Collatz!
[GENUINE] Player 0 got explosive [52] from: 1
 ...generating Collatz!
[INVADER] Sent fake message to player 2 with value 6
[INVADER] Faker 2 sent fake [6] from origin -99 to [GENUINE] Player 2
[GENUINE] Player 4 got explosive [26] from: 0
 ...generating Collatz!
[GENUINE] Player 3 got explosive [13] from: 4
 ...generating Collatz!
[INVADER] Sent fake message to player 4 with value 48
[INVADER] Faker 4 sent fake [48] from origin -99 to [GENUINE] Player 4
[GENUINE] Player 2 got explosive [40] from: 3
 ...generating Collatz!
[GENUINE] Player 1 got explosive [20] from: 2
 ...generating Collatz!
[INVADER] Sent fake message to player 0 with value 3
[INVADER] Faker 0 sent fake [3] from origin -99 to [GENUINE] Player 0
[GENUINE] Player 0 got explosive [10] from: 1
 ...generating Collatz!
[GENUINE] Player 4 got explosive [5] from: 0
 ...generating Collatz!
[INVADER] Sent fake message to player 2 with value 83
[INVADER] Faker 2 sent fake [83] from origin -99 to [GENUINE] Player 2
[GENUINE] Player 3 got explosive [16] from: 4
 ...generating Collatz!
[GENUINE] Player 2 got explosive [8] from: 3
 ...generating Collatz!
[INVADER] Sent fake message to player 0 with value 19
[INVADER] Faker 0 sent fake [19] from origin -99 to [GENUINE] Player 0
[GENUINE] Player 1 got explosive [4] from: 2
 ...generating Collatz!
[GENUINE] Player 0 got explosive [2] from: 1
 ...generating Collatz!
Potato exploded on player: 0
[GENUINE] Player 4 got explosive [5] from: 0
 ...generating Collatz!
[INVADER] Sent fake message to player 2 with value 60
[INVADER] Faker 2 sent fake [60] from origin -99 to [GENUINE] Player 2
[GENUINE] Player 3 got explosive [16] from: 4
 ...generating Collatz!
[GENUINE] Player 2 got explosive [8] from: 3
 ...generating Collatz!
[INVADER] Sent fake message to player 4 with value 70
[INVADER] Faker 4 sent fake [70] from origin -99 to [GENUINE] Player 4
[GENUINE] Player 1 got explosive [4] from: 2
 ...generating Collatz!
[GENUINE] Player 0 got explosive [2] from: 1
[INVADER] Sent fake message to player 2 with value 8
[INVADER] Faker 2 sent fake [8] from origin -99 to [GENUINE] Player 2
[GENUINE] Player 4 got explosive [2] from: 0
 ...generating Collatz!
Potato exploded on player: 4
[GENUINE] Player 3 got explosive [5] from: 4
 ...generating Collatz!
[GENUINE] Player 2 got explosive [16] from: 3
 ...generating Collatz!
[INVADER] Sent fake message to player 3 with value 14
[INVADER] Faker 3 sent fake [14] from origin -99 to [GENUINE] Player 3
[GENUINE] Player 1 got explosive [8] from: 2
 ...generating Collatz!
[GENUINE] Player 0 got explosive [4] from: 1
[INVADER] Sent fake message to player 0 with value 3
[INVADER] Faker 0 sent fake [3] from origin -99 to [GENUINE] Player 0
[GENUINE] Player 4 got explosive [4] from: 0
[GENUINE] Player 3 got explosive [4] from: 4
 ...generating Collatz!
[INVADER] Sent fake message to player 4 with value 92
[INVADER] Faker 4 sent fake [92] from origin -99 to [GENUINE] Player 4
[GENUINE] Player 2 got explosive [2] from: 3
 ...generating Collatz!
Potato exploded on player: 2
[GENUINE] Player 1 got explosive [5] from: 2
 ...generating Collatz!
[GENUINE] Player 0 got explosive [16] from: 1
[INVADER] Sent fake message to player 3 with value 49
[INVADER] Faker 3 sent fake [49] from origin -99 to [GENUINE] Player 3
[GENUINE] Player 4 got explosive [16] from: 0
[GENUINE] Player 3 got explosive [16] from: 4
 ...generating Collatz!
[INVADER] Sent fake message to player 2 with value 29
[INVADER] Faker 2 sent fake [29] from origin -99 to [GENUINE] Player 2
[GENUINE] Player 2 got explosive [8] from: 3
[GENUINE] Player 1 got explosive [8] from: 2
 ...generating Collatz!
[INVADER] Sent fake message to player 3 with value 45
[INVADER] Faker 3 sent fake [45] from origin -99 to [GENUINE] Player 3
[GENUINE] Player 0 got explosive [4] from: 1
[GENUINE] Player 4 got explosive [4] from: 0
[INVADER] Sent fake message to player 3 with value 83
[INVADER] Faker 3 sent fake [83] from origin -99 to [GENUINE] Player 3
[GENUINE] Player 3 got explosive [4] from: 4
 ...generating Collatz!
[GENUINE] Player 2 got explosive [2] from: 3
[INVADER] Sent fake message to player 0 with value 20
[INVADER] Faker 0 sent fake [20] from origin -99 to [GENUINE] Player 0
[GENUINE] Player 1 got explosive [2] from: 2
 ...generating Collatz!
Potato exploded on player: 1
[GENUINE] Player 0 got explosive [5] from: 1
[GENUINE] Player 4 got explosive [5] from: 0
[INVADER] Sent fake message to player 4 with value 2
[INVADER] Faker 4 sent fake [2] from origin -99 to [GENUINE] Player 4
[GENUINE] Player 3 got explosive [5] from: 4
 ...generating Collatz!
The WINNER is player: 3
