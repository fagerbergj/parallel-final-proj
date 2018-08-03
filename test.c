#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "omp.h"
#include <mpi.h>
#include <time.h>

int main(int argc, char* argv){

        MPI_Init(NULL, NULL);
        int rank, numranks;
        MPI_Comm_size(MPI_COMM_WORLD, &numranks);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	for(int i = 0; i < numranks; i++){
		MPI_Barrier(MPI_COMM_WORLD);
		if(rank==i){
			printf("RANK %d WORKING....\n", rank);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}


	 MPI_Finalize();
}

