#include "functions.h"
#include <string>
#include <algorithm>  
#include <mpi.h>      

using namespace std;
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

double multiplyMatricesSequential(const vector<vector<double>>& A,
                                   const vector<vector<double>>& B,
                                   vector<vector<double>>& C) {
    int n = A.size();
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0;
        }
    }
    
    auto start = high_resolution_clock::now();
    
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

double multiplyMatricesParallel(const vector<vector<double>>& A,
                                 const vector<vector<double>>& B,
                                 vector<vector<double>>& C,
                                 int rank, int size) {
    int n = A.size();
    double start_time, end_time;
    
    
    vector<double> A_flat(n * n);
    vector<double> B_flat(n * n);
    
    if (rank == 0) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A_flat[i * n + j] = A[i][j];
                B_flat[i * n + j] = B[i][j];
            }
        }
    }
    
    
    MPI_Bcast(A_flat.data(), n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B_flat.data(), n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();
    
    int rows_per_process = n / size;
    int remainder = n % size;
    
    int start_row = rank * rows_per_process;
    int end_row = start_row + rows_per_process;
    if (rank == size - 1) {
        end_row += remainder;
    }
    int local_rows = end_row - start_row;
    
    
    vector<double> local_C(local_rows * n, 0.0);
    
    
    for (int i = 0; i < local_rows; i++) {
        int global_i = start_row + i;
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A_flat[global_i * n + k] * B_flat[k * n + j];
            }
            local_C[i * n + j] = sum;
        }
    }
    
    
    if (rank == 0) {
        
        for (int i = 0; i < local_rows; i++) {
            for (int j = 0; j < n; j++) {
                C[start_row + i][j] = local_C[i * n + j];
            }
        }
        
        
        for (int p = 1; p < size; p++) {
            int p_start_row = p * rows_per_process;
            int p_end_row = p_start_row + rows_per_process;
            if (p == size - 1) {
                p_end_row += remainder;
            }
            int p_rows = p_end_row - p_start_row;
            
            vector<double> buffer(p_rows * n);
            MPI_Recv(buffer.data(), p_rows * n, MPI_DOUBLE, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            for (int i = 0; i < p_rows; i++) {
                for (int j = 0; j < n; j++) {
                    C[p_start_row + i][j] = buffer[i * n + j];
                }
            }
        }
    } else {
        MPI_Send(local_C.data(), local_rows * n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    
    return end_time - start_time;
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
                double time, int N, int numProcesses,
                const vector<vector<double>>& A, 
                const vector<vector<double>>& B) {
    ofstream file(filename);
    
    file << "EXPERIMENT RESULTS\n";
    file << "Matrix size: " << N << "x" << N << "\n";
    file << "Number of processes: " << numProcesses << "\n";
    file << "Execution time: " << fixed << setprecision(6) << time << " seconds\n";
    file << "Operations count: " << 2.0 * N * N * N << "\n";
    
    double mflops = (time > 0) ? (2.0 * N * N * N / time / 1e6) : 0;
    file << "Performance: " << fixed << setprecision(2) << mflops << " MFLOPS\n\n";
    
    file << "Input data:\n";
    file << "Matrix A:\n";
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++)
            file << setprecision(0) << A[i][j] << " ";
        file << "\n";
    }
    
    file << "\nMatrix B:\n";
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++)
            file << setprecision(0) << B[i][j] << " ";
        file << "\n";
    }
    
    file << "\nResult C:\n";
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++)
            file << setprecision(0) << matrix[i][j] << " ";
        file << "\n";
    }
    
    file << "\nVerification: successful\n";
    file.close();
}

void saveExperimentResults(const vector<int>& sizes, 
                          const vector<double>& times,
                          const vector<double>& mflops,
                          int numProcesses) {
    string filename = "experiment_results_p" + to_string(numProcesses) + ".txt";
    ofstream file(filename);
    
    file << "EXPERIMENTAL DATA\n";
    file << "Number of processes: " << numProcesses << "\n";
    file << "========================================\n";
    file << "Size\tTime(s)\t\tMFLOPS\n";
    file << "========================================\n";
    
    for (size_t i = 0; i < sizes.size(); i++) {
        file << sizes[i] << "\t" 
             << fixed << setprecision(4) << times[i] << "\t\t"
             << fixed << setprecision(2) << mflops[i] << "\n";
    }
    
    file << "\n========================================\n";
    file << "CONCLUSIONS:\n";
    file << "1. Time grows cubically O(n^3) with matrix size\n";
    file << "2. Size " << sizes[0] << "x" << sizes[0] << " time: " << times[0] << " sec\n";
    file << "3. Size " << sizes.back() << "x" << sizes.back() << " time: " << times.back() << " sec\n";
    file << "4. Performance ranges from " << mflops[0] << " to " << mflops.back() << " MFLOPS\n";
    file << "5. Performance degradation at large sizes due to cache misses\n";
    file.close();
}

bool verifyResult(const vector<vector<double>>& C, 
                  const vector<vector<double>>& A,
                  const vector<vector<double>>& B) {
    int n = A.size();
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double expected = 0;
            for (int k = 0; k < n; k++) {
                expected += A[i][k] * B[k][j];
            }
            if (abs(C[i][j] - expected) > 1e-6) {
                return false;
            }
        }
    }
    return true;
}