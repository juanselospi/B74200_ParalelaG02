# Proyecto 4 – Programación Paralela y Concurrente  

**Autores**

Juan Sebastián Loaiza Ospina B74200
Silvio Castillo Morales C38910

---

## Cálculo de la Subsequencia Común Más Larga (LCS) usando MPI

Este proyecto implementa el algoritmo de **Longest Common Subsequence (LCS)** en dos versiones:

- **Versión Serial**
- **Versión Paralela usando MPI (modelo Wavefront)**

El objetivo es comparar ambas implementaciones y analizar si la versión distribuida acelera el cómputo para cadenas grandes.

---

## Descripción General

El programa genera dos secuencias de ADN y calcula su **LCS** mediante programación dinámica.  
La matriz `DP` de tamaño `(n+1) x (m+1)` se llena con la siguiente regla:

- Si los caracteres coinciden:  
  `DP[i][j] = DP[i-1][j-1] + 1`
- Si no coinciden:  
  `DP[i][j] = max(DP[i-1][j], DP[i][j-1])`

La parte más costosa es el llenado completo de la matriz, especialmente para secuencias de miles de caracteres.

---

## Paralelización con MPI

Para la versión paralela se implementó un esquema tipo **wavefront**, donde:

- La matriz se procesa **por diagonales**
- Cada proceso calcula un bloque independiente de la diagonal
- Se intercambian fronteras entre procesos usando:
  - `MPI_Send`
  - `MPI_Recv`
- Se respetan las dependencias `arriba`, `izquierda` y `diagonal`

El proceso 0 imprime los tiempos finales.

---

## Compilación y Ejecución

### Compilar:

- Compilar con: make

Esto genera el ejecutable principal: sequences.out

- Parametros de ejecucion: mpirun -np [hilos] ./sequences.out
 
- Donde:
       hilos       -> cantidad de hilos MPI a usar (int)

- Ejemplo de ejecucion usado para pruebas: mpirun -np 8 ./sequences.out


---

## Resultados Obtenidos

Pruebas realizadas con secuencias de **10000 caracteres**, usando **8 procesos MPI**.

---

LCS serial de tamaño [10000] encontrada:

Tiempo version serial: 5.5457 segundos

LCS paralela con 8 procesos y de tamaño [10000] encontrada:

Tiempo version paralela: 4.9904 segundos

SpeedUp: 1.11128x

---

*Para estos resultados comente la parte que imprime la cadena entera en la terminal, pero esta activado que se vea por defecto*
