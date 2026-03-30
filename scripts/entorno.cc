#include <stdio.h>
#include <omp.h>

int main() {
    #ifdef _OPENMP
        printf("OpenMP está disponible. Versión: %d\n", _OPENMP);
    #else 
        printf("OpenMP NO está disponible en este compilador.\n");
    #endif 
    return 0;
}