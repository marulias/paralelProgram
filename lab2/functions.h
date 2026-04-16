#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <omp.h>

using namespace std;
using namespace std::chrono;

void generateMatrix(vector<vector<double>>& matrix, int n);
bool readMatrix(const string& filename, vector<vector<double>>& matrix);
bool checkSizes(const vector<vector<double>>& A, const vector<vector<double>>& B);
double multiplyMatricesParallel(const vector<vector<double>>& A,
                                 const vector<vector<double>>& B,
                                 vector<vector<double>>& C, int num_threads);
void printMatrix(const vector<vector<double>>& matrix, int maxRows = 10);
void saveResult(const string& filename, const vector<vector<double>>& matrix, 
                double time, int N, int num_threads,
                const vector<vector<double>>& A, 
                const vector<vector<double>>& B);
void saveParallelExperimentResults(const vector<int>& sizes, 
                                   const vector<int>& threads,
                                   const vector<vector<double>>& times,
                                   const vector<vector<double>>& mflops);
double getSpeedup(double time_serial, double time_parallel);
double getEfficiency(double speedup, int num_threads);

#endif