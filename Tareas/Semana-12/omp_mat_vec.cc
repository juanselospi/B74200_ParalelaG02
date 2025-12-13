/**
 *  Producto de una matriz por un vector en paralelo (version OpenMP)
 *  Compilar con:
 *     make omp_mat_vec.out
 *  Ejecutar con:
 *     ./omp_met_vec.out [num_hilos]
 *  [DEBUG]: Compilar con: 
 *     g++ -fopenmp -DDEBUG omp_mat_vec.cc -o omp_mat_vec.out
 *  [DEBUG]: Ejecutar RUNTIME (version por defecto) con:
 *      ./omp_mat_vec.out [num_hilos]
 *  [DEBUG]: Ejecutar STATIC con:
 *      export OMP_SCHEDULE="static"
 *      ./omp_mat_vec.out [num_hilos]
 *  [DEBUG]: Ejecutar DTNAMIC con:
 *      export OMP_SCHEDULE="dynamic"
 *      ./omp_mat_vec.out [num_hilos]
 * 
 *      EJEMPLOS DE RESULTADOS COMENTADOS AL FINAL
 */

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

using namespace std;

/* Global variables */
int     thread_count;
int     m, n;
double* A;
double* x;
double* y;

/* Serial functions */
void Usage(char* prog_name);
void Read_matrix(const char* prompt, double A[], int m, int n);
void Read_vector(const char* prompt, double x[], int n);
void Print_matrix(const char* title, double A[], int m, int n);
void Print_vector(const char* title, double y[], int m);


/* Parallel function */
void omp_mat_vect(void);


/*------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    long thread;
    double start, finish, wused;

    if(argc != 2) {
        Usage(argv[0]);
    }

    thread_count = atoi(argv[1]);

    printf("Enter m and n\n");
    scanf("%d%d", &m, &n);

    A = new double [m*n];
    x = new double [n];
    y = new double [m];

    Read_matrix("Enter the matrix", A, m, n);
    Print_matrix("We read", A, m, n);

    Read_vector("Enter the vector", x, n);
    Print_vector("We read", x, n);

    // borro la creacion y join de pthreads y cambios por OpenMP 
    start = omp_get_wtime();
    omp_mat_vect();
    finish = omp_get_wtime();
    wused = finish - start;

    Print_vector("The product is", y, m);

    printf("\nOpenMP con %ld hilos, wall time = %.3f s\n", thread_count, wused);


    // para new se usa delete, para malloc es free
    delete [] A;
    delete [] x;
    delete [] y;

    return 0;
}  /* main */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   print a message showing what the command line should
 *            be, and terminate
 * In arg :   prog_name
 */
void Usage (char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count>\n", prog_name);
   exit(0);
}  /* Usage */


/*------------------------------------------------------------------
 * Function:    Read_matrix
 * Purpose:     Read in the matrix
 * In args:     prompt, m, n
 * Out arg:     A
 */
void Read_matrix(const char* prompt, double A[], int m, int n) {
   int             i, j;

   printf("%s\n", prompt);
   for (i = 0; i < m; i++) 
      for (j = 0; j < n; j++)
         scanf("%lf", &A[i*n+j]);
}  /* Read_matrix */


/*------------------------------------------------------------------
 * Function:        Read_vector
 * Purpose:         Read in the vector x
 * In arg:          prompt, n
 * Out arg:         x
 */
void Read_vector(const char* prompt, double x[], int n) {
   int   i;

   printf("%s\n", prompt);
   for (i = 0; i < n; i++) 
      scanf("%lf", &x[i]);
}  /* Read_vector */


/*------------------------------------------------------------------
 * Function:    Print_matrix
 * Purpose:     Print the matrix
 * In args:     title, A, m, n
 */
void Print_matrix(const char* title, double A[], int m, int n) {
   int   i, j;

   printf("%s\n", title);
   for (i = 0; i < m; i++) {
      for (j = 0; j < n; j++)
         printf("%4.1f ", A[i*n + j]);
      printf("\n");
   }
}  /* Print_matrix */


/*------------------------------------------------------------------
 * Function:    Print_vector
 * Purpose:     Print a vector
 * In args:     title, y, m
 */
void Print_vector(const char* title, double y[], int m) {
   int   i;

   printf("%s\n", title);
   for (i = 0; i < m; i++)
      printf("%4.1f ", y[i]);
   printf("\n");
}  /* Print_vector */


/*------------------------------------------------------------------
 * Function:       Omp_mat_vect
 * Purpose:        Multiply an mxn matrix by an nx1 column vector
 * Global in vars: A, x, m, n, thread_count
 * Global out var: y
 */
void omp_mat_vect(void) {
    int i, j;

    // paralelization of m*n
    #pragma omp parallel for num_threads(thread_count) private(j) schedule(runtime)
    for(i = 0; i < m; i++) {
        y[i] = 0.0;

        int tid = omp_get_thread_num();

        for(j = 0; j < n; j++) {
            y[i] += A[i*n + j] * x[j];
        }

#ifdef DEBUG
        printf("\n[Hilo %d] procesó la fila %d\n", tid, i);
#endif

    }
}  /* Omp_mat_vect */

/*------------------------------------------------------------------
    RESULTADOS default (runtime)

 I  ~/Desktop/VSCode/Paralela y Concurrente/Semana-12  main !1  g++ -fopenmp -DDEBUG omp_mat_vec.cc -o omp_mat_vec.out                          ✔  20:24:00 
 I  ~/Desktop/VSCode/Paralela y Concurrente/Semana-12  main !1 ?1  ./omp_mat_vec.out 4                                                          ✔  20:24:02 
Enter m and n
6
4
Enter the matrix
1 2 3 4
5 6 7 8
9 10 11 12
2 4 6 8
1 3 5 7
0 2 4 6
We read
 1.0  2.0  3.0  4.0 
 5.0  6.0  7.0  8.0 
 9.0 10.0 11.0 12.0 
 2.0  4.0  6.0  8.0 
 1.0  3.0  5.0  7.0 
 0.0  2.0  4.0  6.0 
Enter the vector
0 1 0 1
We read
 0.0  1.0  0.0  1.0 

[Hilo 0] procesó la fila 0

[Hilo 0] procesó la fila 4

[Hilo 0] procesó la fila 5

[Hilo 2] procesó la fila 1

[Hilo 3] procesó la fila 3

[Hilo 1] procesó la fila 2
The product is
 6.0 14.0 22.0 12.0 10.0  8.0 

OpenMP con 4 hilos, wall time = 0.001 s


    RESULTADOS (static)

 I  ~/Desktop/VSCode/Paralela y Concurrente/Semana-12  main !1 ?1  export OMP_SCHEDULE="static"                                            ✔  48s  20:24:53 

 I  ~/Desktop/VSCode/Paralela y Concurrente/Semana-12  main !1 ?1  ./omp_mat_vec.out 4                                                          ✔  20:28:49 
Enter m and n
6
4
Enter the matrix
1 2 3 4
5 6 7 8
9 10 11 12
2 4 6 8
1 3 5 7
0 2 4 6
We read
 1.0  2.0  3.0  4.0 
 5.0  6.0  7.0  8.0 
 9.0 10.0 11.0 12.0 
 2.0  4.0  6.0  8.0 
 1.0  3.0  5.0  7.0 
 0.0  2.0  4.0  6.0 
Enter the vector
0 1 0 1
We read
 0.0  1.0  0.0  1.0 

[Hilo 0] procesó la fila 0

[Hilo 0] procesó la fila 1

[Hilo 3] procesó la fila 5

[Hilo 2] procesó la fila 4

[Hilo 1] procesó la fila 2

[Hilo 1] procesó la fila 3
The product is
 6.0 14.0 22.0 12.0 10.0  8.0 

OpenMP con 4 hilos, wall time = 0.000 s


    RESULTADOS (dynamic)

 I  ~/Desktop/VSCode/Paralela y Concurrente/Semana-12  main !1 ?1  export OMP_SCHEDULE="dynamic"                                           ✔  44s  20:29:36 

 I  ~/Desktop/VSCode/Paralela y Concurrente/Semana-12  main !1 ?1  ./omp_mat_vec.out 4                                                          ✔  20:31:08 
Enter m and n
6
4
Enter the matrix
1 2 3 4
5 6 7 8
9 10 11 12
2 4 6 8
1 3 5 7
0 2 4 6
We read
 1.0  2.0  3.0  4.0 
 5.0  6.0  7.0  8.0 
 9.0 10.0 11.0 12.0 
 2.0  4.0  6.0  8.0 
 1.0  3.0  5.0  7.0 
 0.0  2.0  4.0  6.0 
Enter the vector
0 1 0 1
We read
 0.0  1.0  0.0  1.0 

[Hilo 0] procesó la fila 0

[Hilo 0] procesó la fila 4

[Hilo 0] procesó la fila 5

[Hilo 1] procesó la fila 3

[Hilo 3] procesó la fila 1

[Hilo 2] procesó la fila 2
The product is
 6.0 14.0 22.0 12.0 10.0  8.0 

OpenMP con 4 hilos, wall time = 0.000 s

 */
