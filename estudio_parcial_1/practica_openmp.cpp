// Este es un script para practicar la librería openmp
// También debo de acostumbrarme a la creación de arreglos dinámicos

#include <iostream>
#include <omp.h>

int main(int argc, char** argv) {
   /*
   NOTAS IMPORTANTES:
   Para acceder a la dirección de memoria de un objeto es necesario usar el operador &
   Un apuntador a un objeto debe ser del mismo tipo que el objeto al que está apuntando 
   Un arreglo dinámico es un arreglo de apuntadores
   Si hay un new entonces debe haber un delete
   Siempre necesitamos mantener el tamaño del arreglo en una variable
   */

   // Creación de un arreglo dinámico
   const int SIZE = 10000;
   int* array = new int[SIZE];

   for(int i = 0; i < SIZE; i++){
      array[i] = 1;    
   }

   for(int i = 0; i < SIZE; i++){
      //std::cout << array[i] << '\n';
   }

   // Ahora lo que quiero hacer es sumar todos los valores de arreglo
   omp_set_num_threads(3);
   int suma = 0;
   
   
   // En este for hay un error muy grave: se están creando condiciones de carrera porque
   // los hilos quieren modificar la misma variable al mismo tiempo
   #pragma omp parallel shared(suma, array, SIZE)
   {
      #pragma omp for schedule(dynamic, 100)
      for(int i = 0; i < SIZE; i++){
         suma += array[i];
      }
   }

   std::cout << "Esta es la suma con condiciones de carrera " << suma << '\n';

   // Vamos a hacerlo ahora sí bien
   // PREGUNTA: porque al usar for schedule(dynamic, 200) me estaba dando condicones de carrera
   int new_suma = 0;
   #pragma omp parallel shared(new_suma, array, SIZE)
   {
      #pragma omp parallel for reduction(+: new_suma)
      for(int i = 0; i < SIZE; i++){
         new_suma += array[i];
      }

   }

   std::cout << "Esta es la suma SIN condiciones de carrera " << new_suma << '\n';

   // Ahora hagamos un ejemplo en el que se haga uso de la cláusula master
   int thread_id;
   #pragma omp parallel shared(array, SIZE) private(thread_id)
   {  
      thread_id = omp_get_thread_num();
      std::cout << thread_id << '\n';
      #pragma omp master
      {
         std::cout << "Si estás viendo esto es porque soy el hilo maestro; mi thread id es: " << thread_id << "\n";
      }
   }

   // Ahora vamos a hacer un ejemplo usando la cláusula single
   #pragma omp parallel shared(array, SIZE) private(thread_id)
   {
      // es imporatnte notar que no siempre le toca al mismo hilo la ejecución, pero al parecer nunca le toca al master
      #pragma omp single
      {
         thread_id = omp_get_thread_num();
         std::cout << "hola, soy el hilo " << thread_id << " y a mi me toco ejecutar la clausula single\n";
      }
   }


   // Ahora vamos a hacer un ejemplo de la cláusula sections
   // PREGUNTA: porque si estoy especificando que resultado es privado y lo imprimo dentro de cada section
   // se termina creando aún así una condición de carrera
   int resultado1 = 0;
   int resultado2 = 0;

   #pragma omp parallel shared(resultado1, resultado2) private(thread_id)
   {
      #pragma omp sections
      {
         #pragma omp section
         {
            thread_id = omp_get_thread_num();
            for(int i = 0; i < 100; i++){
               resultado1 += 1;
            }
            std::cout << "Soy el hilo " << thread_id << " y sumé " << resultado1 << '\n';
         }

         #pragma omp section
         {
            thread_id = omp_get_thread_num();
            for(int i = 0; i < 1000; i++){
               resultado2 += 1;
            }
            std::cout << "Soy el hilo " << thread_id << " y sumé " << resultado2 << '\n';
         }

         #pragma omp section
         {
            thread_id = omp_get_thread_num();
            for(int i = 0; i < 1000; i++){
               resultado2 += 1;
            }
            std::cout << "Soy el hilo " << thread_id << " y sumé " << resultado2 << '\n';
         }
      }
   }

   // Ahora vamos a usar secciones crítica
   // esto que hice abajo es una porquería, básicamente es secuencial pero más tardado
   int suma_critica = 0;

   #pragma omp parallel shared(suma_critica, array, SIZE) private(thread_id)
   {
      #pragma omp for schedule(dynamic, 100)
      for(int i = 0; i < SIZE; i++){
         #pragma omp critical(suma_innecesaria)
         {
            thread_id = omp_get_thread_num();
            // std::cout << "Soy el hilo " << thread_id << " y estoy sumando el numero " << i << '\n';
            suma_critica += array[i];
         }
      }
   }

   std::cout << suma_critica << '\n';




   delete[] array;

   return 0;
}