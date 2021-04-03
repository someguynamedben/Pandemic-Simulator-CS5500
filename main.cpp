#include <iostream>
#include <mpi.h>
#include <stdlib.h>

#include "Person.hpp"
#include "Building.hpp"

#define MCW MPI_COMM_WORLD

using namespace std;

int main(int argc, char **argv){

    int rank, size;
    int data;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    if((rank % 2) == 0){
        //homes
    }else{
        //non-homes
    }

    MPI_Finalize();

    return 0;
}
