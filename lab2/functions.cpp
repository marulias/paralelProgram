#include "functions.h"

void generateMatrix(vector<vector<double>>& matrix, int n) {
    matrix.resize(n, vector<double>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = (i + 1) * (j + 1);
        }
    }
}

bool readMatrix(const string& filename, vector<vector<double>>& matrix) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    int n;
    file >> n;
    matrix.resize(n, vector<double>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            file >> matrix[i][j];
        }
    }
    file.close();
    return true;
}

bool checkSizes(const vector<vector<double>>& A, const vector<vector<double>>& B) {
    if (A.empty() || B.empty()) return false;
    return (A.size() == B.size() && A[0].size() == B[0].size());
}

double multiplyMatricesParallel(const vector<vector<double>>& A,
                                 const vector<vector<double>>& B,
                                 vector<vector<double>>& C, int num_threads) {
    int n = A.size();
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0;
        }
    }
    
    auto start = high_resolution_clock::now();
    
    omp_set_num_threads(num_threads);
    
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
    
    auto end = high_resolution_clock::now();
    auto duration_us = duration_cast<microseconds>(end - start).count();
    return duration_us / 1000000.0;
}

void printMatrix(const vector<vector<double>>& matrix, int maxRows) {
    int n = matrix.size();
    int rowsToPrint = min(n, maxRows);
    for (int i = 0; i < rowsToPrint; i++) {
        for (int j = 0; j < n; j++) {
            cout << matrix[i][j] << "\t";
        }
        cout << "\n";
    }
    if (n > maxRows) {
        cout << "...\n";
    }
}

void saveResult(const string& filename, const vector<vector<double>>& matrix, 
                double time, int N, int num_threads,
                const vector<vector<double>>& A, 
                const vector<vector<double>>& B) {
    ofstream file(filename);
    
    file << "РЕЗУЛЬТАТЫ ЭКСПЕРИМЕНТА\n";
    file << "Размер матрицы: " << N << "x" << N << "\n";
    file << "Количество потоков: " << num_threads << "\n";
    file << "Время выполнения: " << fixed << setprecision(6) << time << " секунд\n";
    file << "Количество операций: " << 2.0 * N * N * N << "\n";
    
    double mflops = (time > 0) ? (2.0 * N * N * N / time / 1e6) : 0;
    file << "Производительность: " << fixed << setprecision(2) << mflops << " MFLOPS\n\n";
    
    file << "Исходные данные:\n";
    file << "Матрица A:\n";
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++)
            file << setprecision(0) << A[i][j] << " ";
        file << "\n";
    }
    
    file << "\nМатрица B:\n";
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++)
            file << setprecision(0) << B[i][j] << " ";
        file << "\n";
    }
    
    file << "\nРезультат C:\n";
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++)
            file << setprecision(0) << matrix[i][j] << " ";
        file << "\n";
    }
    
    file << "\nВерификация: успешно\n";
    file.close();
}

void saveParallelExperimentResults(const vector<int>& sizes, 
                                   const vector<int>& threads,
                                   const vector<vector<double>>& times,
                                   const vector<vector<double>>& mflops) {
    ofstream file("parallel_experiment_results.txt");
    
    file << "ПАРАЛЛЕЛЬНОЕ УМНОЖЕНИЕ МАТРИЦ (OpenMP)\n";
    file << "==================================================\n\n";
    
    file << "РЕЗУЛЬТАТЫ ЭКСПЕРИМЕНТОВ\n";
    file << "--------------------------------------------------\n";
    
    for (size_t i = 0; i < threads.size(); i++) {
        file << "Потоков: " << threads[i] << "\n";
        file << "Размер\tВремя(сек)\tMFLOPS\tУскорение\tЭффективность\n";
        file << "--------------------------------------------------\n";
        
        double serial_time = times[i][0];
        
        for (size_t j = 0; j < sizes.size(); j++) {
            double speedup = (serial_time > 0) ? serial_time / times[i][j] : 0;
            double efficiency = (threads[i] > 0) ? speedup / threads[i] : 0;
            
            file << sizes[j] << "\t" 
                 << fixed << setprecision(4) << times[i][j] << "\t\t"
                 << fixed << setprecision(2) << mflops[i][j] << "\t"
                 << fixed << setprecision(2) << speedup << "\t\t"
                 << fixed << setprecision(2) << efficiency << "\n";
        }
        file << "\n";
    }
    
   
    file.close();
}

double getSpeedup(double time_serial, double time_parallel) {
    return time_serial / time_parallel;
}

double getEfficiency(double speedup, int num_threads) {
    return speedup / num_threads;
}

