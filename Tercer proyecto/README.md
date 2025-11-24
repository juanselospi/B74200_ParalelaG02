# Proyecto 3 – Programación Paralela y Concurrente


**DISCLAMER**
En el codigo existe 1 version paralela comentada, esta version es la forma matematicamente correcta de resolver k-means
pero no es eficiente en tiempo por las cantidades de overhead en OpenMP *igual se dejó a peticion del profe( para potenciales puntos extra )*

El codigo tiene instrucciones claras de como correr la version 2 que es solo `comentar y descomentar las partes señaladas` bajo estas lineas:

-    // EN CASO DE PROBARSE DESCOMENTAR TODO ENTRE LOS // === 

-    // EN CASO DE QUERER PROBAR LA VERSION DE ARRIBA COMENTAR TODO ENTRE LOS // ......

---

## Agrupamiento de N puntos en R clases usando OpenMP (Extensión realizada para soportar 3D)

Este proyecto implementa el algoritmo de **agrupamiento por centros** (similar a K-means) en dos versiones:

- **Versión Serial**
- **Versión Paralela usando OpenMP**

Además, extiende el problema original para trabajar con puntos en **tres dimensiones (x, y, z)**, aumentando la carga computacional y mejorando las posibilidades de paralelización.

---

## Descripción General

El programa agrupa **N puntos** en **R clases** minimizando la **disimilaridad**, definida como la suma de las distancias al cuadrado entre cada punto y el centro de su clase.

El ciclo principal del algoritmo es:

1. Asignar cada punto a una clase inicial  
2. Recalcular los centros como el promedio de los puntos en cada clase  
3. Reasignar puntos al centro más cercano  
4. Repetir hasta que no haya cambios

El proyecto incluye:

- Implementación **serial**
- Implementación **OpenMP**
- Extensión completa del algoritmo a **3 dimensiones**

---

## Extensión a Tres Dimensiones (Z)

El proyecto original trabajaba con puntos en dos dimensiones.  
Se modificaron las clases para soportar el vector completo `(x, y, z)`:

- `Punto::dist2()` ahora calcula la distancia en 3D  
- La clase `VectorPuntos` genera puntos con coordenada Z aleatoria  
- El constructor copia conserva correctamente la componente Z  

Con estas modificaciones, todos los cálculos en el algoritmo pasan automáticamente a 3D.

Esta decisión incrementa la carga computacional de `masCercano()` —la parte más costosa del algoritmo—, y por lo tanto **mejora la eficiencia de la versión paralela** al tener más trabajo útil por iteración.

---

## Compilación y Ejecución

- Compilar: make
- Ejecutar: ./medios.out

**OPCIONAL:** 
./medios.out [hilos] [puntos] [clases] [archivo.eps] [modo]

`hilos`: cantidad de hilos OpenMP
`puntos`: número total de muestras
`clases`: cantidad de centros
`archivo.eps`: nombre de la imagen de salida

`modo`: 0 asignación aleatoria
        1 round-robin


## Resultados Obtenidos

*Para la configuración:*
1,000,000 puntos
17 clases
4 hilos
Modo 0
Dimensión Z activada

*Salida estandar:*
Usando 4 hilos para generar 1000000 puntos, para 17 clases -> salida: ci0117.eps

Tiempo de asignación inicial de puntos (modo 0): 0.008277 s

Valor de la disimilaridad en la solución encontrada 7.31454e+06, con un total de 2790780 cambios
Tiempo total de agrupamiento (version serial): 120.118511 s

Valor de la disimilaridad en la solución encontrada 7.31454e+06, con un total de 2790780 cambios
Tiempo total de agrupamiento (version paralela): 119.467176 s

SpeedUp: 1.0055x

## Análisis de resultados

*Antes de extender el proyecto a 3 dimensiones, la versión paralela obtenía resultados similares o peores que la serial debido a:*

- Poca carga computacional por punto (distancia 2D)
- Overhead de sincronización (atomic, barriers)
- Acceso compartido frecuente

*Tras agregar la dimensión Z, el cálculo de distancia incrementa:*

dx² + dy²  ->  dx² + dy² + dz²

**Esto aumenta la granularidad del trabajo paralelo y amortiza mejor el overhead.**

*Resultado:*
La versión paralela supera a la serial con un pequeño pero real SpeedUp ≈ 1.0055x.


## En este otro resultado que corri en mi PC de escritorio del mismo codigo

juan@SpookX:/mnt/c/VSCode/B74200_ParalelaG02/Tercer proyecto$ make
g++ -g -c Punto.cc
g++ -g -c VectorPuntos.cc
g++ -g -c medios.cc
g++ -g -fopenmp medios.o VectorPuntos.o Punto.o -o medios.out
juan@SpookX:/mnt/c/VSCode/B74200_ParalelaG02/Tercer proyecto$ ./medios.out
Usando 4 hilos para generar 500000 puntos, para 17 clases -> salida: ci0117.eps

Tiempo de asignación inicial de puntos (modo 0): 0.009803 s

Valor de la disimilaridad en la solución encontrada 3.64965e+06, con un total de 1315197 cambios
Tiempo total de agrupamiento (version serial): 28.681818 s

Valor de la disimilaridad en la solución encontrada 3.64965e+06, con un total de 1315197 cambios
Tiempo total de agrupamiento (version paralela): 28.148548 s

SpeedUp: 1.0189x

---

## Documentación ADICIONAL sobre la version paralela 2 **comentada**

*Corriendo una prueba sobre la version paralela 2 comentada se pbtuvo un ligero speedUp con los siguientes parametros*

Usando 4 hilos para generar 100000 puntos, para 17 clases -> salida: paralela.eps

Tiempo de asignación inicial de puntos (modo 0): 0.000721 s

Valor de la disimilaridad en la solución encontrada 730357, con un total de 271693 cambios
Tiempo total de agrupamiento (version serial): 3.093277 s

Valor de la disimilaridad en la solución encontrada 730357, con un total de 271693 cambios
Tiempo total de agrupamiento (version paralela): 3.051403 s

SpeedUp: 1.0137x

*Conclusion*
Es un resultado posible mas no es la norma por el overhead, esto se añade como demostracion de su funcionamiento.
