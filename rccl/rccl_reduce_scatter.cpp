#include <iostream>
#include <vector>
#include <hip/hip_runtime.h>
#include <mpi.h>
#include <rccl/rccl.h>

#define NCCL_CALL(call)                                                                     \
    {                                                                                       \
        ncclResult_t  ncclStatus = call;                                                    \
        if (ncclSuccess != ncclStatus) {                                                    \
            fprintf(stderr,                                                                 \
                    "ERROR: NCCL call \"%s\" in line %d of file %s failed "                 \
                    "with "                                                                 \
                    "%s (%d).\n",                                                           \
                    #call, __LINE__, __FILE__, ncclGetErrorString(ncclStatus), ncclStatus); \
            exit( ncclStatus );                                                             \
        }                                                                                   \
    }

#define CHECK_HIP(cmd)                                                        \
  {                                                                           \
    hipError_t error = cmd;                                                   \
    if (error != hipSuccess) {                                                \
      fprintf(stderr, "error: '%s'(%d) at %s:%d\n", hipGetErrorString(error), \
              error, __FILE__, __LINE__);                                     \
      exit(EXIT_FAILURE);                                                     \
    }                                                                         \
  }

__global__ void scale(int* buff, int len) {

  int tid = blockIdx.x * blockDim.x + threadIdx.x;
  int idx = tid;
  if (idx < len) {
    buff[idx] *= idx;
  }

}

int main(int argc, char* argv[]) {

    int mpi_rank = 0, mpi_size = 1;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    int ndevices, my_device = 0;
    hipGetDeviceCount(&ndevices);
    my_device = mpi_rank % ndevices;
    hipSetDevice(my_device);
    int assigned_device;
    hipGetDevice(&assigned_device);
    std::cout << mpi_rank << ": " << ": my device is " << assigned_device << std::endl;

    ncclUniqueId nccl_uid;
    if (mpi_rank == 0) ncclGetUniqueId(&nccl_uid);
    MPI_Bcast(&nccl_uid, sizeof(ncclUniqueId), MPI_BYTE, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    ncclComm_t nccl_comm;
    ncclCommInitRank(&nccl_comm, mpi_size, nccl_uid, mpi_rank);

    int *recv_buff;
    CHECK_HIP(hipMalloc((void **)&recv_buff, sizeof(int)));
    int *send_buff;
    CHECK_HIP(hipMalloc((void **)&send_buff, sizeof(int) * mpi_size));

    std::vector<int> host_vec{0};
    host_vec.assign(mpi_size, mpi_rank);
    int *host_buff = host_vec.data();

    CHECK_HIP(hipMemcpy(
                send_buff, host_buff, mpi_size * sizeof(int), hipMemcpyHostToDevice));

    hipStream_t stream;
    hipStreamCreate(&stream);

    scale<<<1, mpi_size, 0, stream>>>(send_buff, mpi_size);
    hipStreamSynchronize(stream);

    NCCL_CALL(ncclReduceScatter(
                send_buff, recv_buff, 1, ncclInt, ncclSum, nccl_comm, stream));

    hipStreamSynchronize(stream);

    int msg;
    CHECK_HIP(hipMemcpy(&msg, recv_buff, sizeof(int), hipMemcpyDeviceToHost));
    std::cout << "Final msg on rank " << mpi_rank << " is " << msg << std::endl;

    hipFree(send_buff);
    hipFree(recv_buff);
    ncclCommDestroy(nccl_comm);
    MPI_Finalize();
    return 0;
}