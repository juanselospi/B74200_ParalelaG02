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
- Dirección de giro configurable (horario / antihorario)
- Proceso malicioso que envía mensajes falsos
- Detección y filtrado de mensajes inválidos

---

## Requisitos del Sistema
- Compilador de **C++** con soporte para `pthreads`
- **Make**
- Sistema operativo **Linux/Unix**
- Bibliotecas estándar de C++

---

## Compilación y Ejecución

### Problema de los Monos

- Compilar: make
- Ejecutar: ./bin/Monkeys.out 

OPCIONAL: ./bin/Monkeys.out [N], donde N es el número de monos como parámetro opcional

---

### Problema de la Papa Caliente (Pringles)

- Compilar: make bin/Pringles.out
- Ejecutar: ./bin/Pringles.out

---

**Limpieza**
- Para limpiar los archivos compilados: make clean

RECOMENDADO: Ejecutar make clean después de correr cada ejercicio para mantener el directorio organizado.
