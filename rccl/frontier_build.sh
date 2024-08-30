#!/bin/bash

rm rccl_reduce_scatter.exe

module load craype-accel-amd-gfx90a
#module load cpe/23.12
module load rocm/6.0.0
module load cmake
#module use /autofs/nccs-svm1_sw/crusher/amdsw/modules
#module load rocshmem/1.6.2
export LD_LIBRARY_PATH=${CRAY_LD_LIBRARY_PATH}:${LD_LIBRARY_PATH}
export LDFLAGS="-L${MPICH_DIR}/lib -lmpi ${CRAY_XPMEM_POST_LINK_OPTS} -lxpmem ${PE_MPICH_GTL_DIR_amd_gfx90a} ${PE_MPICH_GTL_LIBS_amd_gfx90a} -I${MPICH_DIR}/include"
export MPICH_GPU_SUPPORT_ENABLED=1

hipcc rccl_reduce_scatter.cpp -o rccl_reduce_scatter.exe -I${MPI_DIR}/include $LDFLAGS -lrccl -Wno-unused-result