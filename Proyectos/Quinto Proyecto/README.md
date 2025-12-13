# Proyecto 5 – Programación Paralela y Concurrente  
## Implementación de K-means en CUDA (Extensión a 3D)

---

## Disclaimer

Este proyecto contiene dos versiones del algoritmo de agrupamiento tipo **K-means**:

- **Versión Serial**
- **Versión CUDA**

Debido a las limitaciones del modelo CUDA, la implementación paralela NO puede usar clases ni métodos complejos dentro del kernel, por lo que se utilizan **arreglos planos** para representar puntos, centros y clases.  
La versión serial se mantiene como referencia y se usa para comparar resultados y tiempos de ejecución.

---

# Agrupamiento de N puntos en R clases usando CUDA (3D)

Este proyecto implementa el algoritmo de **agrupamiento por centros**, extendido completamente a **tres dimensiones (x, y, z)** para aumentar la carga computacional y favorecer la paralelización.

Se comparan ambas versiones en términos de:

- tiempo total
- disimilaridad final
- convergencia
- SpeedUp obtenido

---

## Descripción General del Algoritmo

El algoritmo sigue el ciclo típico de K-means:

1. **Asignación inicial** de puntos a clases  
2. **Recalculo de centros** (promedio de puntos en cada clase)  
3. **Reasignación** de puntos al centro más cercano  
4. Repetir hasta que no haya cambios

La mayor carga computacional es:

- cálculo de distancias punto–centro  
- búsqueda del centro más cercano  

Esta parte es la que se acelera en la GPU.

---

# Implementación CUDA

## Representación de datos

Para adaptarse al modelo de memoria de CUDA, los puntos y centros se convierten a arreglos planos:

- `puntosX`, `puntosY`, `puntosZ`
- `centrosX`, `centrosY`, `centrosZ`
- `clases`

Estos se copian a la memoria de la GPU mediante `cudaMemcpy()`.

---

## Kernel `actualizarPuntosCUDA`

El kernel ejecuta:

- cálculo de distancias entre un punto y todos los centros  
- selección del centro más cercano  
- actualización de la clase del punto  
- conteo global de cambios mediante `atomicAdd()`  

Al finalizar cada iteración:

- se copian las clases de vuelta a CPU  
- se recalculan los centros en CPU  
- los nuevos centros se copian a GPU  
- se repite hasta converger

---

# Compilación y Ejecución

- Compilar: make
- Ejecutar: ./mediosCUDA.out

**OPCIONAL:** 
./mediosCUDA.out [bloque] [puntos] [clases] [archivo.eps] [modo]

`bloque`: tamaño de los bloques cuda *ES recomendado usar: 128, 256 ó 512 segun documentacion de nvidia*
`puntos`: número total de muestras
`clases`: cantidad de centros
`archivo.eps`: nombre de la imagen de salida

`modo`: 0 asignación aleatoria
        1 round-robin

## Resultados Obtenidos

*Salida estandar con bajo numero de puntos*

Usando 256 CUDA para generar 100000 puntos, para 17 clases -> salida: cuda.eps

Tiempo de asignación inicial de puntos (modo 0): 0.000614 s

Valor de la disimilaridad en la solución encontrada 730357, con un total de 271693 cambios
Tiempo total de agrupamiento (version serial): 3.239898 s

Valor de la disimilaridad en la solución encontrada 728739, con un total de 486132 cambios
Tiempo total de agrupamiento (version CUDA): 0.481796 s

SpeedUp: 6.7246x

*Salida estandar con un numero alto de puntos*

Usando 256 CUDA para generar 1000000 puntos, para 17 clases -> salida: cuda.eps

Tiempo de asignación inicial de puntos (modo 0): 0.006308 s

Valor de la disimilaridad en la solución encontrada 7.31454e+06, con un total de 2790780 cambios
Tiempo total de agrupamiento (version serial): 119.198819 s

Valor de la disimilaridad en la solución encontrada 7.29651e+06, con un total de 4587850 cambios
Tiempo total de agrupamiento (version CUDA): 5.165997 s

SpeedUp: 23.0737x

## Análisis de resultados

CUDA escala excelente cuando el numero de puntos crece, gracias a su arquitectura masivamente paralela y a la alta proporción de trabajo útil por hilo.

**NOTA FINAL DEL AUTOR**

Nota en laptops híbridas *AMD + NVIDIA* si la batería baja demasiado, la GPU NVIDIA puede apagarse y los kernels CUDA pueden devolver valores incorrectos. Antes de correr el programa, verificar con nvidia-smi que la dGPU esté activa. 

(,:
