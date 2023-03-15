#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <random>
#include <cmath>

using namespace std;

float** read_csv(const std::string& filename, char delimiter = ',', int* rows = nullptr, int* cols = nullptr) {
    // Open the file
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + filename);
    }

    // Read the data
    std::string line;
    int num_rows = 0, num_cols = 0;
    while (std::getline(file, line)) {
        ++num_rows;
        std::stringstream line_stream(line);
        std::string cell;
        while (std::getline(line_stream, cell, delimiter)) {
            ++num_cols;
        }
    }
    file.clear();
    file.seekg(0);

    float** data = new float*[num_rows];
    for (int i = 0; i < num_rows; ++i) {
        data[i] = new float[num_cols / num_rows];
    }

    int row = 0, col = 0;
    while (std::getline(file, line)) {
        std::stringstream line_stream(line);
        std::string cell;
        while (std::getline(line_stream, cell, delimiter)) {
            data[row][col++] = std::stof(cell);
        }
        col = 0;
        ++row;
    }

    // Set the output values of rows and columns if provided
    if (rows != nullptr) {
        *rows = num_rows;
    }
    if (cols != nullptr) {
        *cols = num_cols / num_rows;
    }

    // Close the file
    file.close();

    return data;
}

void imprime_datos(float** data, int rows, int cols){
    for(int row = 0; row < rows; row++){
        for(int col = 0; col < cols; col++){
            std::cout << data[row][col] << " ";
        }
        std::cout << '\n';
    }
}

float euclidean_distance(float* p1, float* p2, int dimensiones) {
    float sum = 0.0;
    for (int i = 0; i < dimensiones; ++i) {
        sum += pow(p2[i] - p1[i], 2);
    }
    return sqrt(sum);
}

int index_of_min_element(float* nums, int size){
    int index = 0;
    for(int i = 0; i < size; i++){
        if(nums[i] < nums[index]){
            index = i;
        }
    }
    return index;
}

int asigna_centroide(float* punto, float** centroides, int rows_centroides, int cols_centroides){
    // Tenemos que encontrar la distancia de el punto a cada centroide y regresar el índice con la distancia más pequeña
    float* distancias = new float[rows_centroides];

    // Llenamos el arreglo con las distancias a cada centroide
    for(int i = 0; i < rows_centroides; i++){
        distancias[i] = euclidean_distance(punto, centroides[i], cols_centroides);
    }

    // Encontramos el índice con la menor distancia para que ese sea el centroide asignado
    int asignado = index_of_min_element(distancias, rows_centroides);

    delete[] distancias;
    return asignado;
}


int main(){
    // Primero tenemos que hacer la matriz para poder agregar ahí los datos
    int ROWS = 100;
    int COLS = 2;
    int* rows = &ROWS;
    int* cols = &COLS;

    float** datos = new float*[ROWS];
    for(int i = 0; i < ROWS; i++){
        datos[i] = new float[COLS];
    }

    // Aquí podemos trabajar con la matriz
    datos = read_csv("../paralelos2023/100_data.csv", ',', rows, cols);
    // imprime_datos(datos, ROWS, COLS);

    // Seleccionamos aleatoriamente k centroides
    // Recordar que un centroide en este caso tiene dos dimensiones
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0, 1.0);

    int k = 5;
    float** centroides = new float*[k];
    for(int i = 0; i < k; i++){
        centroides[i] = new float[COLS];
    }
    for(int i = 0; i < k; i ++){
        for(int j = 0; j < COLS; j++){
            centroides[i][j] = dist(gen);
        }
    }
    std::cout << "Centroides originales:\n";
    imprime_datos(centroides, k, COLS);

    // Asignamos cada dato al centroide más cercano
    int* asignados = new int[ROWS];
    for(int i = 0; i < ROWS; i++){
        asignados[i] = asigna_centroide(datos[i], centroides, k, COLS); 
    }

    // Movemos cada centroide a la media de sus puntos asignados
    // Primero encontramos los puntos que están asignados a cada centroide
    // Luego les calculo la media sobre cada eje y ese es el nuevo centroide

    for(int aux_centroide = 0; aux_centroide < k; aux_centroide++){
        int total_encontrados = 0;
        float* sumas = new float[COLS];
        // Primero sumamos y contamos cuantos datos vamos encontrando para cada centroide
        for(int i = 0; i < ROWS; i++){
            // Encontramos la media de los valores y ya
            if(asignados[i] = aux_centroide){
                total_encontrados += 1;
                for(int j = 0; j < COLS; j++){
                    sumas[j] += datos[i][j];
                }
            }
        }
        // Ahora movemos los centroides
        std::cout << total_encontrados << '\n';
        if(total_encontrados != 0){
            for(int i = 0; i < COLS; i++){
                centroides[aux_centroide][i] = sumas[i] / total_encontrados;
            } 
        } else {
            // Si no encontramos ningún centroide, lo movemos aleatoriamente
            for(int i = 0; i < COLS; i++){
                centroides[aux_centroide][i] = dist(gen);
            }
        }
        delete[] sumas;
    }


    std::cout << "Centroides movidos:\n";
    imprime_datos(centroides, k, COLS);





    


    


    // Cerramos la memoria de la matriz
    for(int i = 0; i < ROWS; i++){
        delete[] datos[i];
    }
    delete[] datos;
    for(int i = 0; i < k; i++){
        delete[] centroides[i];
    } 
    delete[] centroides;
}