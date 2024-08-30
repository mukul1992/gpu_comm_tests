#!/bin/bash

#SBATCH -A CSC589
#SBATCH -J rccl_test
#SBATCH -o rccl_test.out
#SBATCH -t 05:00
#SBATCH -p batch
#SBATCH -q debug
#SBATCH -N 1

srun -n 8 ./rccl_reduce_scatter.exe
