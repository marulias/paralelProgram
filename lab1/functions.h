#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <omp.h>

using namespace std;
using namespace std::chrono;

bool readMatrix(const string& filename, vector<vector<double>>& matrix);
bool checkSizes(const vector<vector<double>>& A, const vector<vector<double>>& B);

double multiplyMatricesParallel(const vector<vector<double>>& A,
    const vector<vector<double>>& B,
    vector<vector<double>>& C, int num_threads);

void printMatrix(const vector<vector<double>>& matrix);
void saveResult(const string& filename, const vector<vector<double>>& matrix, 
                double time, int N, int num_threads,
                const vector<vector<double>>& A, 
                const vector<vector<double>>& B);

#endif