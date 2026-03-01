#include <iostream>
#include <iomanip>
#include "functions.h"

using namespace std;

int main() {
    
    
    ifstream fileA("matrix_A.txt");
    int N;
    fileA >> N;
    fileA.close();

    vector<vector<double>> A(N, vector<double>(N));
    vector<vector<double>> B(N, vector<double>(N));
    vector<vector<double>> C(N, vector<double>(N, 0));

    if (!readMatrix("matrix_A.txt", A)) return 1;
    if (!readMatrix("matrix_B.txt", B)) return 1;

    if (!checkSizes(A, B)) {
        cout << "Error: matrix sizes do not match\n";
        return 1;
    }

    cout << "Размер матриц: " << N << "x" << N << "\n\n";
    
   
    double time = multiplyMatricesParallel(A, B, C, 2);
    
    cout << "Время выполнения: " << fixed << setprecision(6) << time << " сек\n";
    cout << "Количество операций: " << 2.0 * N * N * N << "\n";
    cout << "Производительность: " << (2.0 * N * N * N / time / 1e6) << " MFLOPS\n\n";
    
    cout << "Результат умножения:\n";
    printMatrix(C);
    
    
    saveResult("result.txt", C, time, N, 2, A, B);
    cout << "\nРезультат сохранен в result.txt\n";
    
   
    return 0;
}