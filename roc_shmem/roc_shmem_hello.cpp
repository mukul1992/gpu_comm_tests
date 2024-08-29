#include <iostream>
#include <roc_shmem.hpp>

using namespace rocshmem;

__global__ void simple_shift(int *destination) {
    int mype = roc_shmem_my_pe();
    int npes = roc_shmem_n_pes();
    int peer = (mype + 1) % npes;

    printf("%d: peer is %d \n", mype, peer);

    roc_shmem_int_p(destination, mype, peer);
}

int main(int argc, char* argv[]) {

    int mpi_rank = 0, mpi_size = 1;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    int ndevices, my_device = 0;
    hipGetDeviceCount(&ndevices);
    my_device = mpi_rank % ndevices;
    std::cout << mpi_rank << ": my device should be " << my_device << std::endl;
    hipSetDevice(my_device);
    roc_shmem_init();
    int rank = roc_shmem_my_pe();

    int assigned_device;
    hipGetDevice(&assigned_device);
    std::cout << mpi_rank << ": " << rank << ": my device is " << assigned_device << std::endl;

    int *destination = (int *) roc_shmem_malloc(sizeof(int));

    hipStream_t stream;
    hipStreamCreate(&stream);
    simple_shift<<<1, 1, 0, stream>>>(destination);
    hipStreamSynchronize(stream);
    //roc_shmem_barrier_all();

    int msg;
    hipMemcpyAsync(&msg, destination, sizeof(int), hipMemcpyDeviceToHost, stream);
    hipStreamSynchronize(stream);

    std::cout << rank << ": received message " << msg << std::endl;

    roc_shmem_free(destination);
    roc_shmem_finalize();
    MPI_Finalize();

    return 0;
}
