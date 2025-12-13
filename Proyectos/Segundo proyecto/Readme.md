# Proyecto 2 - Programación Paralela y Concurrente

## Descripción
Este proyecto implementa un contador de etiquetas HTML utilizando programación paralela con hilos (pthreads). El programa analiza archivos HTML, cuenta la frecuencia de cada etiqueta válida y compara el rendimiento entre la versión serial y paralela.

## Características
- Procesamiento paralelo de múltiples archivos HTML simultáneamente
- Verificación de etiquetas HTML válidas
- Procesamiento en bloques de máximo 512 bytes
- Comparativa de rendimiento entre versiones serial y paralela
- Soporte para múltiples( o el mismo varias veces si se incluye multiples veces ne la lista ) archivos de entrada

## Requisitos del Sistema
- Compilador de **C++** con soporte para `pthread`
- **Make**
- Sistema operativo **Linux**
- Bibliotecas estándar de C++

## Compilación y Ejecucion
*Compilar*
make

*Ejecutar* 
ejemplo: ./bin/contar.out -t=100 prueba4.html prueba2.html prueba5.html -e=1,4,3

`-t=`: Número de contadores
`archivo.html`: Archivos por procesar
`-e=`: estrategias: 1 estatica por bloques, 2 estatica ciclica, 3 dinamica, 4 personalizada(bloques + dinamica)

*Limpieza*
make clean

### Ejemplo de salida
Sorting...
Files: 3
Using: 100 counters per file

Global tag count:
a : 464
abbr : 2
b : 2
blockquote : 38
body : 42
br : 2
button : 40
cite : 2
code : 40
dd : 6
div : 796
dl : 2
dt : 6
em : 40
footer : 42
form : 39
head : 42
header : 40
html : 41
i : 2
img : 1
input : 59
label : 194
li : 519
main : 40
meta : 40
nav : 40
ol : 40
option : 151
p : 589
script : 38
section : 204
select : 38
source : 1
span : 2
strong : 42
style : 40
table : 40
tbody : 40
td : 626
textarea : 38
th : 158
thead : 40
title : 42
tr : 198
u : 38
ul : 81
var : 2
video : 2

Wall time (serial): 646.155 ms
Main CPU time (serial): 0.661 s

Wall time (parallel): 111.750 ms
Main CPU time (parallel): 1.669 s

SpeedUp: x5.782

## Análisis de Rendimiento
Eficiencias Significativas: 
Las mejoras de rendimiento más notorias se observan cuando se procesan archivos grandes con un alto número de hilos. En particular:

Archivo prueba5.html: Con 10,000+ líneas de código HTML, este archivo es suficientemente grande para sobrecargar la versión serial del programa.

100 hilos: Para esta cantidad de contadores, el procesamiento paralelo demuestra una aceleración significativa (SpeedUp de ~5.78x en el ejemplo).

Archivos pequeños: Para archivos con pocas líneas de código, las ventajas del paralelismo son menos evidentes debido al overhead de gestión de hilos.
