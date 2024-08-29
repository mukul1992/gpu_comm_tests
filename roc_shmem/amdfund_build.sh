#!/bin/bash

rm roc_shmem_hello.o roc_shmem_hello

module use /share/bpotter/modulefiles
module load rocshmem

hipcc ./roc_shmem_hello.cpp -I${ROCSHMEM_DIR}_1.6.3/include -I${MPI_DIR}/include -fgpu-rdc -o ./roc_shmem_hello.o -c
hipcc ./roc_shmem_hello.o ${ROCSHMEM_DIR}_1.6.3/lib64/librocshmem.a -L${MPI_DIR}/lib -lmpi -lmlx5 -libverbs -lhsa-runtime64 -fgpu-rdc -o roc_shmem_hello
