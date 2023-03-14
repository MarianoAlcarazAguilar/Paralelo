#include <iostream>
#include <omp.h>
#include <limits>
#include <climits>

int main() {
    int vector_size = 100;
    int* a = new int[vector_size];
    int* b = new int[vector_size];
    int* c = new int[vector_size];

    #pragma omp parallel for
    for(int i = 0; i < vector_size; i++){
        a[i] = b[i] = i+1;
    }

    int suma = 0;

    #pragma omp parallel for reduction (+:suma)
    for(int i = 0; i < vector_size; i++){
        suma = a[i] + b[i];
        c[i] = a[i] + b[i];
    }

    std::cout << "El total fue: " << suma << "\n";

    /* PROBLEMA DEL MÍNIMO */
    int minimo = INT_MAX; // Este es el número entero más grande que existe en la arquitectura de la computadora; viene de la librería limits
    // Para el reduction siempre va el operador y luego el nombre de la variable donde se va a guardar
    #pragma omp parallel for reduction (min:minimo)
    for(int i = 0; i < vector_size; i++){
        if((a[i] + b[i]) <= minimo){
            minimo = a[i] + b[i];
        }
    }

    std::cout << "El minimo fue: " << minimo << "\n";

    /* PROBLEMA DE ENCONTRAR PARES */
    bool encontrado = false;
    int valor_a_buscar = 3;
    // este || operador es or
    #pragma omp parallel for reduction (||:encontrado)
    for(int i = 0; i < vector_size; i++){
        if(c[i] == valor_a_buscar){
            encontrado = true;
        }
    }

    std::cout << "encontramos el valor 3? " << encontrado;


    delete[] a;
    delete[] b;
    delete[] c;
    

    return 0;
}