# Declarar y usar estructuras de datos

**📘 Estructuras de datos comunes en C/C++ para Programación Paralela (OpenMP y MPI)**

En programación paralela, especialmente con OpenMP y MPI, las estructuras de datos deben ser simples, contiguas en memoria y fáciles de dividir entre hilos o procesos.
Las más recomendadas son:

## 1️⃣ Arreglos simples (1D arrays)
Declaración estática
int arr[100];

Declaración dinámica
int *arr = (int*) malloc(N * sizeof(int));

Inicialización
for (int i = 0; i < N; i++)
    arr[i] = rand() % 100;

Por qué es útil en OpenMP/MPI:

✔ Se puede dividir fácilmente en segmentos
✔ Son contiguos en memoria → excelente para rendimiento
✔ Scatter, gather y reduce funcionan naturalmente sobre 1D arrays

## 2️⃣ Vectores (usando std::vector)
#include <vector>

std::vector<int> v(100);       // tamaño 100
v[0] = 10;


Ventajas:

Manejan memoria automáticamente

Son seguros y convenientes

Puedes obtener el puntero a los datos con v.data()

Ejemplo:

int *ptr = v.data();


(Esto sirve mucho para usar con MPI, que necesita punteros crudos.)

## 3️⃣ Matrices (2D)

En C, una matriz es realmente un arreglo lineal con mapeo manual.

Forma 1: matriz estática
int A[ROWS][COLS];

Forma 2 (más usada en MPI/OpenMP): matriz lineal 1D
int *A = (int*) malloc(ROWS * COLS * sizeof(int));

Acceso como 2D:
A[i * COLS + j] = 10;


¿Por qué se usa esta forma?
✔ Es contigua en memoria
✔ Scatter/Broadcast funciona directo
✔ OpenMP paraleliza fácil con #pragma omp parallel for

## 4️⃣ Structs básicos (para agrupar datos)

Los struct permiten agrupar varios valores relacionados.
Esto es útil cuando un hilo o proceso debe manejar una entidad completa.

Ejemplo:

typedef struct {
    int x;
    int y;
    int z;
} Punto;


Acceso:

Punto p;
p.x = 10;


En arreglos:

Punto puntos[100];
puntos[0].x = 5;


⚠ IMPORTANTE para MPI
Si vas a enviar struct por MPI, debes definir un MPI_Datatype.
(Para examen normalmente no se pide, pero es bueno saberlo.)

## 5️⃣ Buffer circular (para productor-consumidor)

Muy usado en ejercicios clásicos de concurrencia.

int buffer[BUFFER_SIZE];
int inPos = 0;
int outPos = 0;
int count = 0;


Inserción:

buffer[inPos] = valor;
inPos = (inPos + 1) % BUFFER_SIZE;
count++;


Extracción:

valor = buffer[outPos];
outPos = (outPos + 1) % BUFFER_SIZE;
count--;

## 6️⃣ Datos que suelen ser globales en OpenMP

Variables típicas:

int arreglo[N];
int sumaGlobal = 0;
int maximoGlobal;


Paralelización:

#pragma omp parallel for reduction(+:sumaGlobal)
for (int i = 0; i < N; i++)
    sumaGlobal += arreglo[i];


O con atomic:

if (localMax > maximoGlobal)
    #pragma omp atomic write
    maximoGlobal = localMax;

## 7️⃣ Datos compartidos en MPI

MPI recomienda usar arreglos planos (1D):

int *subArray = malloc(chunk * sizeof(int));
MPI_Scatter(arr, chunk, MPI_INT, subArray, chunk, MPI_INT, 0, MPI_COMM_WORLD);


Matrices también:

int *A = malloc(N*N*sizeof(int));



# OpenMP

## 🧠 2. Modelo mental de OpenMP

**OpenMP trabaja en paralelismo compartido:**

Todos los hilos comparten el mismo espacio de memoria

Cada hilo tiene su ID (omp_get_thread_num())

Puedes decidir qué variables son:

shared (compartidas)

private (copias locales)

reduction (combina resultados)

*Ideal para:*

✔ Arreglos
✔ Algoritmos que pueden dividirse en bloques
✔ Computación numérica
✔ Bucles independientes

## ✂️ 5. Dividir manualmente un arreglo entre hilos

Ejemplo típico:

#pragma omp parallel num_threads(T)
{
    int tid = omp_get_thread_num();
    int start = tid * (N / T);
    int end = (tid == T - 1) ? N : start + (N / T);
}


*Casos de examen:*

Máximo local → máximo global

Suma manual sin reduction

Cálculo distribuido por bloques

## 🛑 6. Zonas críticas: #pragma omp critical

Para proteger secciones donde solo un hilo puede entrar a la vez.

#pragma omp critical
{
    total += x;
}


Casos de uso:

Productor–Consumidor

Lectores–Escritores

Actualizaciones de estructuras compartidas

Impresión ordenada

## ⚡ 7. Atomic vs Critical

atomic → rápido, para operaciones simples sobre una variable

#pragma omp atomic
maxNum = localMax;

critical → más general pero más lento
#pragma omp critical
{ total += local; }

## 🔄 8. Reducciones: reduction()

La forma MÁS LIMPIA de combinar valores de varios hilos:

long suma = 0;

#pragma omp parallel for reduction(+:suma)
for (int i = 0; i < N; i++)
    suma += A[i];


## 🚧 9. Barreras: #pragma omp barrier

Fuerza a todos los hilos a esperar.

#pragma omp barrier


Útil cuando:

Todos deben terminar una fase antes de continuar

Se sincronizan etapas (ej: reads antes de writes)

Simulaciones paso a paso


## 🎭 10. Secciones: #pragma omp sections

Cuando diferentes hilos deben ejecutar tareas distintas:

#pragma omp parallel sections
{
    #pragma omp section
    tareaA();

    #pragma omp section
    tareaB();
}

Casos típicos:

Una parte calcula A, otra calcula B

Preprocesado + cómputo + salida

Pipelines simples


## 🧱 SECCIÓN EXTRA PARA AÑADIR AL README – OpenMP Locks

OpenMP no tiene semáforos reales, pero sí tiene locks (bloqueos explícitos), que permiten exclusión mutua muy granular.

🔐 OpenMP Locks

Los locks permiten controlar manualmente cuándo un hilo entra a una sección crítica.

🌟 Declaración
omp_lock_t lock;
omp_init_lock(&lock);

🔒 Tomar el lock
omp_set_lock(&lock);


Si otro hilo tiene el lock → este hilo espera.

🔓 Liberar el lock
omp_unset_lock(&lock);

🗑 Destruir el lock
omp_destroy_lock(&lock);

*🧪 Ejemplo clásico (similar a mutex en pthreads)*

long total = 0;
omp_lock_t lk;

omp_init_lock(&lk);

#pragma omp parallel
{
    long local = 0;

    for(int i = 0; i < 1000; i++)
        local++;

    omp_set_lock(&lk);
    total += local;
    omp_unset_lock(&lk);
}

omp_destroy_lock(&lk);

printf("Total = %ld\n", total);


**✔ ¿Cuándo usar locks?**

Cuando necesitas exclusión mutua parcial, no toda una sección crítica

Cuando un hilo debe esperar explícitamente a un evento

Cuando necesitás controlar manualmente el acceso a estructuras complejas

*🔥 Son ideales para:*

Productor–Consuidor manual (OpenMP versión con locks)

Lectores–Escritores con prioridad

Estructuras de datos dinámicas (listas, colas, árboles)


## ⏱ SECCIÓN EXTRA – Medición de tiempo en OpenMP

La mejor forma en OpenMP es:

⏱ omp_get_wtime()

Devuelve tiempo en segundos (doble precisión).

*Ejemplo:*

double t0 = omp_get_wtime();

#pragma omp parallel for
for(int i = 0; i < N; i++)
    A[i] = B[i] * 2;

double t1 = omp_get_wtime();

printf("Tiempo: %f segundos\n", t1 - t0);

*✔ Ventajas*

Funciona igual en Windows, Linux, Mac

No requiere <sys/time.h>

Mide tiempo real (wall-clock)

Ideal para comparar OpenMP vs serial


# MPI

*🧪 ¿Qué deberías entender de **Scatter-Gather** para el examen?*

Tu profe puede preguntarte:

1. ¿Para qué sirve MPI_Scatter?

Divide un arreglo del proceso 0 en bloques iguales y los reparte a todos los procesos.

2. ¿Para qué sirve MPI_Reduce?

Combina valores locales de todos los procesos (sumas, máximos, mínimos, etc.) y deja el resultado final en el proceso root.

3. ¿Por qué cada proceso calcula su propia suma?

Porque MPI permite paralelizar el trabajo dividiendo datos.

4. ¿Qué pasa si N no es divisible entre nproc?

Debes implementar un manejo adicional (padding o distribución irregular).
Pero en exámenes casi siempre usan valores perfectos para evitar complicaciones.

# VALGRIND

**🔍 Guía rápida de Valgrind para detectar fugas de memoria**

Valgrind es una herramienta que permite analizar programas en C/C++ y encontrar:

Fugas de memoria (memory leaks)

Lecturas/escrituras inválidas

Uso de memoria no inicializada

Doble free()

Accesos fuera de rango

Errores de punteros

Lo más usado para depuración es memcheck.

## ✅ 1. Compilar con símbolos de depuración

Esto hace que los errores tengan líneas de código identificables.

g++ -g miPrograma.cc -o miPrograma.out


⚠️ IMPORTANTE: No uses optimizaciones (-O2, -O3) cuando uses Valgrind.

## ✅ 2. Ejecutar el programa con Valgrind
valgrind --leak-check=full ./miPrograma.out


Esto muestra:

fugas detectadas

dónde fueron hechas las reservas (new, malloc)

qué memoria no se liberó

## 📝 3. Opciones más útiles
🔸 Mostrar todos los detalles posibles
valgrind --leak-check=full --show-leak-kinds=all ./miPrograma.out


Esto te muestra fugas:

definitively lost (memoria perdida → ERROR grave)

indirectly lost

still reachable (a veces aceptable según el programa)

possibly lost

🔸 Mostrar en qué línea ocurrió el error
valgrind --track-origins=yes ./miPrograma.out


Esto ayuda cuando hay memoria no inicializada.

🔸 Suprimir spam de errores de librerías externas
valgrind --quiet ./miPrograma.out

## 🟦 4. Interpretación rápida del resultado

Ejemplo típico:

HEAP SUMMARY:
    in use at exit: 40 bytes in 1 blocks
    total heap usage: 5 allocs, 4 frees, 100 bytes allocated

40 bytes in 1 blocks are definitely lost in loss record 1 of 1
   at 0x4C2FB55: malloc
   by 0x4006AF: funcionX() (archivo.cc:32)
   by 0x4007E8: main (archivo.cc:58)

Significado:

definitely lost → fuga real

indirectly lost → apunta a algo perdido

still reachable → no es grave, pero no está limpio

0 bytes in 0 blocks → ✔ tu programa no tiene fugas

## 🟢 5. Resultado ideal

Valgrind debe terminar con:

All heap blocks were freed -- no leaks are possible


Esto significa que:

✔ Todas tus llamadas new, malloc fueron liberadas
✔ No dejaste memoria sin liberar
✔ No hay accesos erróneos ni double free

## 🧪 6. Probar un programa con argumentos

Si tu programa toma argumentos:

valgrind --leak-check=full ./miPrograma.out 1000 archivo.txt

## 🧹 7. Valgrind para multihilos

Si usas pthreads (como en muchos de tus proyectos):

valgrind --tool=memcheck --leak-check=full --track-origins=yes --pthread-fds=yes ./miPrograma.out
