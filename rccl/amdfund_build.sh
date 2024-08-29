#!/bin/bash

rm rccl_reduce_scatter

hipcc rccl_reduce_scatter.cpp -o rccl_reduce_scatter -I${MPI_DIR}/include -L${MPI_DIR}/lib -lmpi -lrccl
