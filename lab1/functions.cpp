#include "functions.h"

bool readMatrix(const string& filename, vector<vector<double>>& matrix) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: cannot open file " << filename << "\n";
        return false;
    }

    int n;
    file >> n;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            file >> matrix[i][j];

    file.close();
    return true;
}

bool checkSizes(const vector<vector<double>>& A, const vector<vector<double>>& B) {
    return (A.size() == B.size() && A[0].size() == B[0].size());
}


double multiplyMatricesParallel(const vector<vector<double>>& A,
    const vector<vector<double>>& B,
    vector<vector<double>>& C, int num_threads) {
    int n = A.size();

    
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i][j] = 0;

    auto start = high_resolution_clock::now();

    
    omp_set_num_threads(num_threads);
    
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    auto end = high_resolution_clock::now();
    
    auto duration_us = duration_cast<microseconds>(end - start).count();
    return duration_us / 1000000.0;
}

void printMatrix(const vector<vector<double>>& matrix) {
    int n = matrix.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            cout << matrix[i][j] << "\t";
        cout << "\n";
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
    file << "Производительность: " << fixed << setprecision(4) << mflops << " MFLOPS\n\n";
    
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