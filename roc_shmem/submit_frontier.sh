#!/bin/bash

#SBATCH -A CSC589
#SBATCH -J rocshmem_test
#SBATCH -o rocshmem_test.out
#SBATCH -t 05:00
#SBATCH -p batch
#SBATCH -q debug
#SBATCH -N 1

srun -n 8 ./roc_shmem_hello.exe
