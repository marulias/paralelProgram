#include <iostream>
#include <iomanip>
#include <vector>
#include "functions.h"

using namespace std;

int main() {
    vector<int> sizes = {200, 400, 800, 1200, 1600, 2000};
    vector<double> experimentTimes;
    vector<double> experimentMflops;
    
    cout << "========================================\n";
    cout << "ЭКСПЕРИМЕНТЫ ПО ПЕРЕМНОЖЕНИЮ МАТРИЦ\n";
    cout << "========================================\n\n";
    
    for (int N : sizes) {
        cout << "Размер матрицы: " << N << "x" << N << endl;
        
        vector<vector<double>> A(N, vector<double>(N));
        vector<vector<double>> B(N, vector<double>(N));
        vector<vector<double>> C(N, vector<double>(N));
        
        generateMatrix(A, N);
        generateMatrix(B, N);
        
        cout << "Выполняется умножение (последовательное)... " << flush;
        
        double time = multiplyMatricesSequential(A, B, C);
        double mflops = (2.0 * N * N * N / time / 1e6);
        
        experimentTimes.push_back(time);
        experimentMflops.push_back(mflops);
        
        cout << "Готово\n";
        cout << "Время: " << fixed << setprecision(4) << time << " сек\n";
        cout << "Производительность: " << fixed << setprecision(2) << mflops << " MFLOPS\n\n";
        
        if (N == 200) {
            saveResult("result_200.txt", C, time, N, A, B);
            cout << "Результат для размера 200x200 сохранен в result_200.txt\n\n";
        }
        
        if (!verifyResult(C, A, B)) {
            cout << "ОШИБКА: Верификация не пройдена!\n";
            return 1;
        }
    }
    
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
    
    saveExperimentResults(sizes, experimentTimes, experimentMflops);
    cout << "\nРезультаты экспериментов сохранены в experiment_results.txt\n";
    cout << "\nВСЕ ЭКСПЕРИМЕНТЫ ЗАВЕРШЕНЫ УСПЕШНО\n";
    
    return 0;
}