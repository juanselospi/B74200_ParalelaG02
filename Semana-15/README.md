# Tarea Semanal 15: CUDA

## Archivos de la tarea
Se hicieron 4 archivos, add.cu, add_block.cu, add_grid.cu, y add_grid_memo.cu.

**add.cu**
Versión CUDA más simple, usando 1 bloque y 1 hilo (<<<1,1>>>).
Esta versión funciona, pero es completamente serial.

**add_block.cu**
Se modifica el kernel para trabajar con 256 hilos dentro de un solo bloque.
Los hilos reparten la carga usando threadIdx.x y un stride de blockDim.x.

**add_grid.cu**
Versión mejorada usando múltiples bloques y hilos.
Se implementa el patrón grid-stride loop, recomendado por NVIDIA para escalar a muchos hilos.

**add_grid_memo.cu**
Versión que utiliza prefetching de memoria usando cudaMemPrefetchAsync,
para que la GPU cargue las páginas antes de ejecutar el kernel.

---

## Profiling
- Cada versión fue analizada con el profiler oficial de NVIDIA usando:

nsys profile -t cuda --stats=true ./[nombre_archivo]

./nsys_easy ./[nombre_archivo]

*Esto permitió ver cuánto tiempo consume cada kernel y cuántas migraciones de memoria ocurren.*

## Salidas

**add.cu**

*nsys profile -t cuda --stats=true ./add_cuda*

Collecting data...
Max error: 0
Generating '/tmp/nsys-report-b41f.qdstrm'
[1/6] [========================100%] report1.nsys-rep
[2/6] [========================100%] report1.sqlite
[3/6] Executing 'cuda_api_sum' stats report

 Time (%)  Total Time (ns)  Num Calls    Avg (ns)      Med (ns)     Min (ns)    Max (ns)    StdDev (ns)            Name         
 --------  ---------------  ---------  ------------  ------------  ----------  -----------  ------------  ----------------------
     56.7      117,995,854          2  58,997,927.0  58,997,927.0      24,335  117,971,519  83,401,253.6  cudaMallocManaged     
     43.0       89,441,694          1  89,441,694.0  89,441,694.0  89,441,694   89,441,694           0.0  cudaDeviceSynchronize 
      0.1          307,629          2     153,814.5     153,814.5     147,137      160,492       9,443.4  cudaFree              
      0.1          185,840          1     185,840.0     185,840.0     185,840      185,840           0.0  cuLibraryLoadData     
      0.0           70,292          1      70,292.0      70,292.0      70,292       70,292           0.0  cudaLaunchKernel      
      0.0              882          1         882.0         882.0         882          882           0.0  cuModuleGetLoadingMode
      0.0              431          1         431.0         431.0         431          431           0.0  cuKernelGetName       
      0.0              150          1         150.0         150.0         150          150           0.0  cuLibraryGetKernel    

[4/6] Executing 'cuda_gpu_kern_sum' stats report

 Time (%)  Total Time (ns)  Instances    Avg (ns)      Med (ns)     Min (ns)    Max (ns)   StdDev (ns)             Name           
 --------  ---------------  ---------  ------------  ------------  ----------  ----------  -----------  --------------------------
    100.0       89,433,431          1  89,433,431.0  89,433,431.0  89,433,431  89,433,431          0.0  add(int, float *, float *)

[5/6] Executing 'cuda_gpu_mem_time_sum' stats report

 Time (%)  Total Time (ns)  Count  Avg (ns)  Med (ns)  Min (ns)  Max (ns)  StdDev (ns)               Operation              
 --------  ---------------  -----  --------  --------  --------  --------  -----------  ------------------------------------
     68.3          736,932     48  15,352.8   4,655.5     1,887    83,295     23,701.6  [CUDA memcpy Unified Host-to-Device]
     31.7          342,580     24  14,274.2   3,455.5     1,215    81,600     23,643.4  [CUDA memcpy Unified Device-to-Host]

[6/6] Executing 'cuda_gpu_mem_size_sum' stats report

 Total (MB)  Count  Avg (MB)  Med (MB)  Min (MB)  Max (MB)  StdDev (MB)               Operation              
 ----------  -----  --------  --------  --------  --------  -----------  ------------------------------------
      8.389     48     0.175     0.033     0.004     1.044        0.304  [CUDA memcpy Unified Host-to-Device]
      4.194     24     0.175     0.033     0.004     1.044        0.307  [CUDA memcpy Unified Device-to-Host]

Generated:
	/home/juanselospi/Desktop/VSCode/Paralela y Concurrente/Semana-15/report1.nsys-rep
	/home/juanselospi/Desktop/VSCode/Paralela y Concurrente/Semana-15/report1.sqlite

*./nsys_easy ./add_cuda*

Collecting data...
Max error: 0
Generating '/tmp/nsys-report-48f6.qdstrm'
[1/1] [========================100%] nsys_easy.nsys-rep
Generated:
	/home/juanselospi/Desktop/VSCode/Paralela y Concurrente/Semana-15/nsys_easy.nsys-rep
Generating SQLite file nsys_easy.sqlite from nsys_easy.nsys-rep
Processing [nsys_easy.sqlite] with [/usr/local/cuda-13.0/nsight-systems-2025.3.2/host-linux-x64/reports/cuda_gpu_sum.py]... 

 ** CUDA GPU Summary (Kernels/MemOps) (cuda_gpu_sum):

 Time (%)  Total Time (ns)  Instances    Avg (ns)      Med (ns)     Min (ns)    Max (ns)   StdDev (ns)   Category                 Operation              
 --------  ---------------  ---------  ------------  ------------  ----------  ----------  -----------  -----------  ------------------------------------
     98.8       88,937,304          1  88,937,304.0  88,937,304.0  88,937,304  88,937,304          0.0  CUDA_KERNEL  add(int, float *, float *)          
      0.8          737,958         48      15,374.1       4,463.5       1,887      83,647     23,763.4  MEMORY_OPER  [CUDA memcpy Unified Host-to-Device]
      0.4          342,322         24      14,263.4       3,343.5       1,279      81,760     23,675.9  MEMORY_OPER  [CUDA memcpy Unified Device-to-Host]


**add_block.cu**

*./nsys_easy ./add_block*

Collecting data...
Max error: 0
Generating '/tmp/nsys-report-5854.qdstrm'
[1/1] [========================100%] nsys_easy.nsys-rep
Generated:
	/home/juanselospi/Desktop/VSCode/Paralela y Concurrente/Semana-15/nsys_easy.nsys-rep
Generating SQLite file nsys_easy.sqlite from nsys_easy.nsys-rep
Processing [nsys_easy.sqlite] with [/usr/local/cuda-13.0/nsight-systems-2025.3.2/host-linux-x64/reports/cuda_gpu_sum.py]... 

 ** CUDA GPU Summary (Kernels/MemOps) (cuda_gpu_sum):

 Time (%)  Total Time (ns)  Instances   Avg (ns)     Med (ns)    Min (ns)   Max (ns)   StdDev (ns)   Category                 Operation              
 --------  ---------------  ---------  -----------  -----------  ---------  ---------  -----------  -----------  ------------------------------------
     71.0        2,632,208          1  2,632,208.0  2,632,208.0  2,632,208  2,632,208          0.0  CUDA_KERNEL  add(int, float *, float *)          
     19.8          732,164         48     15,253.4      4,239.0      1,887     83,328     23,756.5  MEMORY_OPER  [CUDA memcpy Unified Host-to-Device]
      9.2          342,033         24     14,251.4      3,343.5      1,279     81,599     23,647.8  MEMORY_OPER  [CUDA memcpy Unified Device-to-Host]


**add_grid.cu**

*./nsys_easy ./add_grid*

Collecting data...
Max error: 0
Generating '/tmp/nsys-report-163a.qdstrm'
[1/1] [========================100%] nsys_easy.nsys-rep
Generated:
	/home/juanselospi/Desktop/VSCode/Paralela y Concurrente/Semana-15/nsys_easy.nsys-rep
Generating SQLite file nsys_easy.sqlite from nsys_easy.nsys-rep
Processing [nsys_easy.sqlite] with [/usr/local/cuda-13.0/nsight-systems-2025.3.2/host-linux-x64/reports/cuda_gpu_sum.py]... 

 ** CUDA GPU Summary (Kernels/MemOps) (cuda_gpu_sum):

 Time (%)  Total Time (ns)  Instances   Avg (ns)     Med (ns)    Min (ns)   Max (ns)   StdDev (ns)   Category                 Operation              
 --------  ---------------  ---------  -----------  -----------  ---------  ---------  -----------  -----------  ------------------------------------
     63.8        1,924,239          1  1,924,239.0  1,924,239.0  1,924,239  1,924,239          0.0  CUDA_KERNEL  add(int, float *, float *)          
     24.8          748,086         58     12,898.0      4,047.0      1,791     80,735     21,051.8  MEMORY_OPER  [CUDA memcpy Unified Host-to-Device]
     11.4          342,288         24     14,262.0      3,375.5      1,279     81,599     23,646.3  MEMORY_OPER  [CUDA memcpy Unified Device-to-Host]


**add_grid_memo.cu**

*./nsys_easy ./add_grid_memo*

Collecting data...
Max error: 0
Generating '/tmp/nsys-report-b766.qdstrm'
[1/1] [========================100%] nsys_easy.nsys-rep
Generated:
	/home/juanselospi/Desktop/VSCode/Paralela y Concurrente/Semana-15/nsys_easy.nsys-rep
Generating SQLite file nsys_easy.sqlite from nsys_easy.nsys-rep
Processing [nsys_easy.sqlite] with [/usr/local/cuda-13.0/nsight-systems-2025.3.2/host-linux-x64/reports/cuda_gpu_sum.py]... 

 ** CUDA GPU Summary (Kernels/MemOps) (cuda_gpu_sum):

 Time (%)  Total Time (ns)  Instances  Avg (ns)   Med (ns)   Min (ns)  Max (ns)  StdDev (ns)   Category                 Operation              
 --------  ---------------  ---------  ---------  ---------  --------  --------  -----------  -----------  ------------------------------------
     64.4          664,188          4  166,047.0  166,047.0   166,015   166,079         37.0  MEMORY_OPER  [CUDA memcpy Unified Host-to-Device]
     33.2          341,938         24   14,247.4    3,359.5     1,247    81,599     23,649.8  MEMORY_OPER  [CUDA memcpy Unified Device-to-Host]
      2.5           25,344          1   25,344.0   25,344.0    25,344    25,344          0.0  CUDA_KERNEL  add(int, float *, float *)          
