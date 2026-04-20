#include <iostream>
#include <iomanip>
#include <vector>
#include <mpi.h>
#include "functions.h"

using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    vector<int> sizes = {200, 400, 800, 1200, 1600, 2000};
    vector<double> experimentTimes;
    vector<double> experimentMflops;
    
    if (rank == 0) {
        cout << "========================================\n";
        cout << "ЭКСПЕРИМЕНТЫ ПО ПЕРЕМНОЖЕНИЮ МАТРИЦ (MPI)\n";
        cout << "Количество процессов: " << size << "\n";
        cout << "========================================\n\n";
    }
    
    for (int N : sizes) {
        if (rank == 0) {
            cout << "Размер матрицы: " << N << "x" << N << endl;
        }
        
        vector<vector<double>> A(N, vector<double>(N));
        vector<vector<double>> B(N, vector<double>(N));
        vector<vector<double>> C(N, vector<double>(N));
        
        if (rank == 0) {
            generateMatrix(A, N);
            generateMatrix(B, N);
            cout << "Выполняется умножение (параллельное, MPI)... " << flush;
        }
        
        double time = multiplyMatricesParallel(A, B, C, rank, size);
        
        if (rank == 0) {
            double mflops = (2.0 * N * N * N / time / 1e6);
            
            experimentTimes.push_back(time);
            experimentMflops.push_back(mflops);
            
            cout << "Готово\n";
            cout << "Время: " << fixed << setprecision(4) << time << " сек\n";
            cout << "Производительность: " << fixed << setprecision(2) << mflops << " MFLOPS\n\n";
            
            if (N == 200) {
                saveResult("result_200_mpi_p" + to_string(size) + ".txt", C, time, N, size, A, B);
                cout << "Результат для размера 200x200 сохранен\n\n";
            }
            
            if (!verifyResult(C, A, B)) {
                cout << "ОШИБКА: Верификация не пройдена!\n";
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
    }
    
    if (rank == 0) {
        cout << "========================================\n";
        cout << "ИТОГОВЫЕ РЕЗУЛЬТАТЫ ЭКСПЕРИМЕНТОВ\n";
        cout << "========================================\n";
        cout << "Размер\tВремя(сек)\tMFLOPS\n";
        cout << "----------------------------------------\n";
        
        for (size_t i = 0; i < sizes.size(); i++) {
            cout << sizes[i] << "\t" 
                 << fixed << setprecision(4) << experimentTimes[i] << "\t\t"
                 << fixed << setprecision(2) << experimentMflops[i] << "\n";
        }
        
        saveExperimentResults(sizes, experimentTimes, experimentMflops, size);
        cout << "\nРезультаты экспериментов сохранены\n";
        cout << "\nВСЕ ЭКСПЕРИМЕНТЫ ЗАВЕРШЕНЫ УСПЕШНО\n";
    }
    
    MPI_Finalize();
    return 0;
}