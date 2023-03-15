#include <omp.h>
#include <iostream>
#include <time.h>

int main() {
    // Indica el tamaño de la cartera de clientes
    long long int numero_clientes = 100000000;

    // Creamos la cartera de clientes
    int* scores_crediticios {new int[numero_clientes]};

    // Cambia la semilla para generar diferentes números aleatorios
    srand(time(NULL));

    // Crea aleatoriamente los scores crediticios de la cartera de clientes
    for(long long int i = 0; i < numero_clientes; i++){
        scores_crediticios[i] = rand() % 100 + 1;
    }

    double start, end;
    long long int malos = 0, buenos = 0;
    start = omp_get_wtime();
    omp_set_num_threads(2);
    #pragma omp parallel shared(buenos, malos, numero_clientes, scores_crediticios)
    {
        #pragma omp parallel for reduction(+:buenos) reduction(+:malos)
        for(long long int i = 0; i < numero_clientes; i++){
            if(scores_crediticios[i] <= 50){
                malos += 1;
            } else {
                buenos += 1;
            }
        }
    }
    end = omp_get_wtime();
    double time = end - start;


    std::cout << "Tiempo en ejecución paralela: " << time << '\n';
    std::cout << "\tClientes buenos encontrados: " << buenos << "\n\tClientes malos encontrados: " << malos << '\n';

    start = omp_get_wtime();
    malos = 0;
    buenos = 0;
    for(long long int i = 0; i < numero_clientes; i++){
        if(scores_crediticios[i] <= 50){
            malos += 1;
        } else {
            buenos += 1;
        }
    }
    end = omp_get_wtime();
    time = end - start;

    std::cout << "Tiempo en ejecución serial: " << time << '\n';
    std::cout << "\tClientes buenos encontrados: " << buenos << "\n\tClientes malos encontrados: " << malos << '\n';


    // itam.examen@gmail.com


    delete[] scores_crediticios;
}