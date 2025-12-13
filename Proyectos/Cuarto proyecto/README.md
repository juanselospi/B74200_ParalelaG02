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

- Parametros de ejecucion: mpirun -np [hilos] ./sequences.out [cadena1] [cadena2]
 
- Donde:
       hilos       -> cantidad de hilos MPI a usar (int)
       cadena1     -> tamaño cadena 1
       cadena2     -> tamaño cadena 2

- Ejemplo de ejecucion usado para pruebas: mpirun -np 8 ./sequences.out 10000 10000 

---

## Resultados Obtenidos

Pruebas realizadas con secuencias de **10000 caracteres**, usando **8 procesos MPI**.

---

LCS serial para cadenas de tamaños: 10000 y 10000 encontrada:
Longitud de la LCS serial: 6519
Tiempo version serial: 3.46411 segundos

LCS paralela por MPI para cadenas de tamaños: 10000 y 10000 encontrada:
Longitud de la LCS paralela: 6519
Tiempo version paralela: 3.03735 segundos

SpeedUp: 1.1405x

---

*Para estos resultados comente la parte que imprime la cadena entera en la terminal, pero esta activado que se vea por defecto*

## Tabla de resultados de rendimiento (MPI)

Pruebas realizadas en mi laptop con **8 procesos MPI** y cadenas aleatorias del mismo tamaño:

| Tamaño S1/S2 | Procesos MPI | Tiempo serial (s) | Tiempo paralelo (s)  | SpeedUp |
|--------------|--------------|-------------------|----------------------|---------|
|  6000 / 6000 |      4       |    1.20994        |       1.09613        | 1.10383 |
|  8000 / 8000 |      4       |    2.17529        |       1.93043        | 1.12684 |
| 10000 /10000 |      8       |    3.46411        |       3.03735        | 1.1405  |
| 12000 /12000 |      8       |    5.23006        |       4.67867        | 1.11785 |
| 15000 /15000 |      8       |    8.27486        |       8.14151        | 1.01638 |
