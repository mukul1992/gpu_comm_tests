#!/bin/bash

rm roc_shmem_hello.o roc_shmem_hello.exe

module load craype-accel-amd-gfx90a
#module load cpe/23.12
module load rocm/6.0.0
module load cmake
module use /autofs/nccs-svm1_sw/crusher/amdsw/modules
module load rocshmem/1.6.2
export LD_LIBRARY_PATH=${CRAY_LD_LIBRARY_PATH}:${LD_LIBRARY_PATH}
export LDFLAGS="-L${MPICH_DIR}/lib -lmpi ${CRAY_XPMEM_POST_LINK_OPTS} -lxpmem ${PE_MPICH_GTL_DIR_amd_gfx90a} ${PE_MPICH_GTL_LIBS_amd_gfx90a} -I${MPICH_DIR}/include"
export MPICH_GPU_SUPPORT_ENABLED=1

hipcc ./roc_shmem_hello.cpp -I${ROCSHMEM_DIR}/include -I${MPICH_DIR}/include -fgpu-rdc -Wno-unused-result -o ./roc_shmem_hello.o -c
hipcc ./roc_shmem_hello.o ${ROCSHMEM_DIR}/lib64/librocshmem.a -lhsa-runtime64 -fgpu-rdc $LDFLAGS -lpthread -o roc_shmem_hello.exe