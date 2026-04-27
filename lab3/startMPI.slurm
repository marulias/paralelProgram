#!/bin/bash
#SBATCH --job-name=matMulMPI
#SBATCH --time=10:00:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --partition=batch
#SBATCH --output=matMul_%j.out

module load intel/mpi4

# Компиляция
mpicxx -std=c++11 -O2 -o laba main.cpp functions.cpp

# Запуск
mpirun -np $SLURM_NTASKS ./laba