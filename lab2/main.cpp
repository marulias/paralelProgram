#include <iostream>
#include <iomanip>
#include <vector>
#include <omp.h>
#include "functions.h"

using namespace std;

int main() {
    vector<int> sizes = {200, 400, 800, 1200, 1600, 2000};
    vector<int> threads = {1, 2, 4, 8};
    
    int max_threads = omp_get_max_threads();
    cout << "Доступное количество ядер: " << max_threads << "\n\n";
    
    vector<vector<double>> experimentTimes;
    vector<vector<double>> experimentMflops;
    
    cout << "========================================\n";
    cout << "ПАРАЛЛЕЛЬНОЕ УМНОЖЕНИЕ МАТРИЦ (OpenMP)\n";
    cout << "========================================\n\n";
    
    for (int num_threads : threads) {
        if (num_threads > max_threads) {
            cout << "Пропуск " << num_threads << " потоков (доступно только " << max_threads << ")\n\n";
            continue;
        }
        
        cout << "ЭКСПЕРИМЕНТ С " << num_threads << " ПОТОКАМИ\n";
        cout << "----------------------------------------\n";
        
        vector<double> times;
        vector<double> mflopsList;
        
        for (int N : sizes) {
            cout << "Размер: " << N << "x" << N << " ... " << flush;
            
            vector<vector<double>> A(N, vector<double>(N));
            vector<vector<double>> B(N, vector<double>(N));
            vector<vector<double>> C(N, vector<double>(N));
            
            generateMatrix(A, N);
            generateMatrix(B, N);
            
            double time = multiplyMatricesParallel(A, B, C, num_threads);
            double mflops = (2.0 * N * N * N / time / 1e6);
            
            times.push_back(time);
            mflopsList.push_back(mflops);
            
            cout << "Готово\n";
            cout << "  Время: " << fixed << setprecision(4) << time << " сек\n";
            cout << "  MFLOPS: " << fixed << setprecision(2) << mflops << "\n";
            
            if (N == 200 && num_threads == 2) {
                saveResult("result_parallel_200.txt", C, time, N, num_threads, A, B);
                cout << "  Результат сохранен в result_parallel_200.txt\n";
            }
            
        }
        
        experimentTimes.push_back(times);
        experimentMflops.push_back(mflopsList);
        
        cout << "\n";
    }
    
    cout << "========================================\n";
    cout << "СВОДНАЯ ТАБЛИЦА РЕЗУЛЬТАТОВ\n";
    cout << "========================================\n\n";
    
    for (size_t t = 0; t < threads.size(); t++) {
        if (threads[t] > max_threads) continue;
        
        cout << "Потоков: " << threads[t] << "\n";
        cout << "Размер\tВремя(сек)\tMFLOPS\n";
        cout << "----------------------------------------\n";
        
        for (size_t s = 0; s < sizes.size(); s++) {
            cout << sizes[s] << "\t" 
                 << fixed << setprecision(4) << experimentTimes[t][s] << "\t\t"
                 << fixed << setprecision(2) << experimentMflops[t][s] << "\n";
        }
        cout << "\n";
    }
    
    saveParallelExperimentResults(sizes, threads, experimentTimes, experimentMflops);
    cout << "Результаты сохранены в parallel_experiment_results.txt\n";
    cout << "\nВСЕ ЭКСПЕРИМЕНТЫ ЗАВЕРШЕНЫ УСПЕШНО\n";
    
    return 0;
}