
## Tabla comparativa para mi algoritmo de multiplicación de matrices

	┌────────┬────────┬────────┬────────┬────────┐
	│ Trabaj │ Serial          │PThreads         │ OpenMP           │  MPI   │
	├────────┼────────┼────────┼────────┼────────┤
	│    2   │1.291990e-03│     7.729530e-04     │7.059574e-04      │6.155440e-04│
	├────────┼────────┼────────┼────────┼────────┤
	│    4   │1.291990e-03│     5.340576e-04      4.711151e-04      │3.992570e-04│
	├────────┼────────┼────────┼────────┼────────┤
	│    8   │1.291990e-03│     7.691383e-04      8.831024e-04      │4.617180e-02│
	├────────┼────────┼────────┼────────┼────────┤
	│   16   │1.291990e-03│     7.479191e-04      1.169920e-03      │2.640818e-01│
	├────────┼────────┼────────┼────────┼────────┤
	│   64   │1.291990e-03│     1.770973e-03      2.149105e-03      │2.049025e+01│
	└────────┴────────┴────────┴────────┴────────┘
	Tiempo empleado en segundos (ms) para n términos
    Las funciones corrieron con matrices de 1000x1000

## PARTE 4

[B74200@poas Semana-14]$ mpiexec -n 4 ./mpi_output.out
Proc 0 of 4 > Does anyone have a toothpick?
Proc 1 of 4 > Does anyone have a toothpick?
Proc 2 of 4 > Does anyone have a toothpick?
Proc 3 of 4 > Does anyone have a toothpick?
[B74200@poas Semana-14]$ 


*version cambiando el -n a 16*

[B74200@poas Semana-14]$ mpiexec -n 16 ./mpi_output.out
Proc 8 of 16 > Does anyone have a toothpick?
Proc 14 of 16 > Does anyone have a toothpick?
Proc 0 of 16 > Does anyone have a toothpick?
Proc 2 of 16 > Does anyone have a toothpick?
Proc 5 of 16 > Does anyone have a toothpick?
Proc 9 of 16 > Does anyone have a toothpick?
Proc 3 of 16 > Does anyone have a toothpick?
Proc 4 of 16 > Does anyone have a toothpick?
Proc 11 of 16 > Does anyone have a toothpick?
Proc 12 of 16 > Does anyone have a toothpick?
Proc 13 of 16 > Does anyone have a toothpick?
Proc 6 of 16 > Does anyone have a toothpick?
Proc 7 of 16 > Does anyone have a toothpick?
Proc 10 of 16 > Does anyone have a toothpick?
Proc 15 of 16 > Does anyone have a toothpick?
Proc 1 of 16 > Does anyone have a toothpick?
[B74200@poas Semana-14]$ 



## PARTE 5

[B74200@poas Semana-14]$ nano mpi_output.mpi 
[B74200@poas Semana-14]$ sbatch mpi_output.mpi Submitted batch job 3845 
[B74200@poas Semana-14]$ ls -l ci0117* -rw-r--r-- 1 B74200 estudiantes 880 Nov 21 14:24 ci0117.3845.out 
[B74200@poas Semana-14]$ cat ci0117.3845.out [prun] Master compute host = compute-1 [prun] Resource manager = slurm [prun] Launch cmd = mpiexec.hydra -bootstrap slurm ./mpi_output.out (family=mpich) 
Proc 1 of 16 > Does anyone have a toothpick? 
Proc 2 of 16 > Does anyone have a toothpick? 
Proc 0 of 16 > Does anyone have a toothpick? 
Proc 3 of 16 > Does anyone have a toothpick? 
Proc 13 of 16 > Does anyone have a toothpick? 
Proc 15 of 16 > Does anyone have a toothpick? 
Proc 12 of 16 > Does anyone have a toothpick? 
Proc 14 of 16 > Does anyone have a toothpick? 
Proc 4 of 16 > Does anyone have a toothpick? 
Proc 6 of 16 > Does anyone have a toothpick? 
Proc 5 of 16 > Does anyone have a toothpick? 
Proc 7 of 16 > Does anyone have a toothpick? 
Proc 8 of 16 > Does anyone have a toothpick? 
Proc 9 of 16 > Does anyone have a toothpick? 
Proc 10 of 16 > Does anyone have a toothpick? 
Proc 11 of 16 > Does anyone have a toothpick? 
[B74200@poas Semana-14]$