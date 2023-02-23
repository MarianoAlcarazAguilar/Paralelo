#include <iostream>

// Solo corre main()
// argc es el número de parámetros, argv son los strings que le vamos a pasar
int main(int argc, char** argv) {

   /* ------------------------------ INTRODUCCIÓN APUNTADORES ------------------------------ */
   if(false){
      // Creamos un objeto
      int dato = 20;
      // Vamos a definir un apuntador a un entero
      // El apuntador tiene que ser del mismo tipo que lo que está apuntando
      // Con el operador & obtenemos la dirección de memoria del objeto
      int* apuntador = &dato;
      // std::endl es como si pusiéramos "\n", pero esto elimina el primero elimina el flush para que sea más eficiente
      std::cout << "Dato " << dato << std::endl;
      std::cout << "Dirección de datos " << &dato << std::endl;
      // Con el * le decimos que nos de el dato en sí al que está apuntando el apuntador
      std::cout << "Dato viniendo del apuntador " << *apuntador << std::endl;
      std::cout << "Dirección de dato " << apuntador << std::endl;
      std::cout << "Dirección del apuntador " << &apuntador << std::endl;

      dato = 44;
      std::cout << "Dato " << dato << std::endl;
      std::cout << "Dirección de datos " << &dato << std::endl;
      std::cout << "Dato viniendo del apuntador " << *apuntador << std::endl;
      std::cout << "Dirección de dato " << apuntador << std::endl;
      std::cout << "Dirección del apuntador " << &apuntador << std::endl;

      // También podemos modificar el dato original a través del apuntador
      *apuntador = 88;
      std::cout << "Dato " << dato << std::endl;
      std::cout << "Dirección de datos " << &dato << std::endl;
      std::cout << "Dato viniendo del apuntador " << *apuntador << std::endl;
      std::cout << "Dirección de dato " << apuntador << std::endl;
      std::cout << "Dirección del apuntador " << &apuntador << std::endl;
   }

   
   /* ------------------------------ ARRAYS ------------------------------ */
   if(false){
      int numeros[5];// = {1,2,3,4,5};
      // Esto te va a dar el número de bytes que tiene tu arreglo
      int aux_1 = sizeof(numeros);
      // Esto te da el número de bytes que tiene cada entero
      int aux_2 = sizeof(int);
      // Para encontrar el tamaño del arreglo; dividimos; y ya
      int longitud = sizeof(numeros) / sizeof(int);
      std::cout << "Tamaño del arreglo " << longitud << std::endl;

      for (int i = 0; i < 5; i++){
         std::cout << numeros[i] << std::endl;
      }
      // Esto solo funciona para arreglos estáticos
      for (int num:numeros) {
         std::cout << num << std::endl;
      }
   }
   
   /* ------------------------------ARREGLOS DINÁMICOS ------------------------------ */
   if(false){
      // Ahora vamos a definir de forma dinámica
      // Tenemos una variable de tipo constante, aunque en realidad puede ser ahorita de cualquier tipo
      const int N = 5;

      /*
      Aquí vamos a definir el arreglo de forma dinámica
      Si veo un new tiene que venir acompañado de un delete en algún lugar en el código.
      Este está en el heap, a diferencia de los no dinámicos que están en el stack
      Este es un arreglo de apuntadores
      NEW implica reservar memoria dinámicamente; por lo tanto, siempre tenemos que eliminar esa memoria reservada.
      */
      int* numeros_dinamicos = new int[N] {11, 12, 13, 14, 15};

      for(int i = 0; i < N; i++){
         // std::cout << numeros_dinamicos[i] << "\n";
      } 

      // Aquí los eliminamos eventualmente. NO SE NOS DEBE OLVIDAR.
      delete[] numeros_dinamicos;
   }
   
   /* ------------------------------ MATRICES ------------------------------ */
   if(false){
      int ren = 2;
      int col = 4;
      // Vamos a tener apuntadores (*) de apuntadores (*) --> **
      int** matriz = new int*[ren]; // Creamos el primer arreglo (de apuntadores)
      for(int i = 0; i < ren; i++){
         matriz[i] = new int[col]; // Agregamos el arreglo (de apuntadores) a cada apuntador
      }

      // En el medio es donde podemos trabajar
      std::cout << *matriz << "\n";

      for(int i = 0; i < ren; i++){
         std::cout << matriz[i] << "\n";
         for(int j = 0; j < col; j++){
            std::cout << &matriz[i][j] << " ";
         }
         std::cout << "\n";
      }

      // Ahora eliminamos los espacios de memoria que usó la matriz
      for(int i = 0; i < ren; i++){
         delete[] matriz[i];
      }
      delete[] matriz;
   }

   /* ------------------------------ INPUT FROM TERMINAL ------------------------------ */
   if(true){
      // Preguntamos a ver si nos pasaron algún valor desde la consola
      if(argc == 1) { // i.e. si no hay parámetros
         std::cout << "No introdujiste parámetros " << argv[0] << "\n";
      } else { // i.e. sí hay parámetros
         for(int i = 1; i < argc; i++){
            std::cout << "Parámetro " << i << " " << argv[i] << "\n";
         }
      }
   }

   return 0;
}


