#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "omp.h"
#include <mpi.h>
#include <time.h>

int totalSize;
int dim;
int n;

int getX(int pos){
	return pos/dim;
}

int getY(int pos){
	return pos%dim;
}

int validSelfMove(int* section, int num){
	for(int i = 0; i < dim; i++){
		if(section[i] == num) return 1;
	}
	return 0;
}

int* genPuzzle(){
	//set random seed
	srand(time(NULL));

	//all 0s
	int* puzzle = (int*)malloc(totalSize*sizeof(int));
	for(int i = 0; i < totalSize; i++){
		puzzle[i] = 0;
	}

	return puzzle;
}

//completed: print matrix separating regions
void printPuzzle(int* puzzle){
	for(int i = 0; i < dim; i++){
		if(i%n == 0 && i !=0 && i != dim-1) {
			for(int u = 0; u < dim*2+n*2; u++) printf("_");
			printf("\n");
		}
		for(int j = 0; j < dim; j++){
			if(j%n == 0 && j !=0 && j!= dim-1) {
				printf(" | ");
			}
			printf("%d ", puzzle[dim*i + j]);
		}
		printf("\n");
	}

}

int main(int argc, char* argv){

	MPI_Init(NULL, NULL);
        int rank, numranks;
        MPI_Comm_size(MPI_COMM_WORLD, &numranks);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//size of inner squares n*n
	n =6;
	//number of row/col in puzzle
	dim = n*n;
	//total number of elements in puzzle dim*dim
	totalSize = dim*dim;
	int* puzzle = (int*)malloc(totalSize*sizeof(int));


	if(rank == 0){
		puzzle = genPuzzle();

		printPuzzle(puzzle);

		//send section n to rank n
		for(int r = 0; r < numranks; r++){
			int* s = (int*)malloc(dim*sizeof(int));
			for(int i = 0; i < n; i++){
				for(int j = 0; j < n; j++){
					//multipliers to map small cubes to full puzzle index
					int xoff = (r/n)* n*n*n;
					int yoff = (r%n)* n;
					//puzzle pos = xoffset to current position + position in smaller section + yoffset to current position
					int puzzlePos = xoff + (i*dim + j) + yoff;
					s[n*i+j] = puzzle[puzzlePos];
					//printf("%d ", s[n*i+j]);
				}
				//printf("\n");
			}
			//printf("\n");
			//send section to rank x
			MPI_Send(s, dim, MPI_INT, r, 0, MPI_COMM_WORLD);
			//rank 0 no longer needs this section
			free(s);
		}
	}

	//each rank recieves their section
	int* section = (int*)malloc(dim*sizeof(int));

	MPI_Recv(section, dim, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

/*	for(int i = 0; i < numranks; i++){
		if(rank == i){
			printf("*****RANK: %d's SECTION*****\n", rank);
			for(int i = 0; i < n; i++){
				for(int j = 0; j < n; j++){
					printf("%d ", section[i*n+j]);
				}
				printf("\n");
			}
		}
	}*/
	//now start solving each qn


	//setup row/col comm
	MPI_Comm row_comm;
	int row_proc=rank/n;

	MPI_Comm_split(MPI_COMM_WORLD, row_proc, rank, &row_comm );


	MPI_Comm col_comm;
	int col_proc=rank%n;

	MPI_Comm_split(MPI_COMM_WORLD, col_proc, rank, &col_comm );

	// for 0 -> n*n - 1
		// for each rank: r
			//rank r then places a number into first available spot
				//rank r checks to see if number is in Qr
				//rank r askes all ranks horizontal if they have that num in the selected row
				//rank r askes all ranks vertical if they have that num in the selected column
			//if valid move, go to next rank
			//if invalid move pick another number

	for(int r = 0; r < numranks; r++){
		MPI_Barrier(MPI_COMM_WORLD);
		if(rank == r){
			//printf("\nrank == %d", r);
			for(int i = 0; i < dim; i++){
				//printf("\nsection[i] = %d\n",section[i]);
				if(section[i] == 0){
					for(int num = 1; num <= dim; num++){
						if(validSelfMove(section, num) == 0){     //if valid
							//printf("%d ", num);
							section[i] = num;
							num = n*n*n;
						}
					}
				}
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Send(section, dim, MPI_INT, 0, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);


	if(rank == 0){
		for(int r = 0; r < numranks; r++){
		//decode indivdual sections
		int* s = malloc(dim*sizeof(int));
			MPI_Recv(s, dim, MPI_INT, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for(int i = 0; i < n; i++){
				for(int j = 0; j < n; j++){
					int xoff = (r/n)* n*n*n;
					int yoff = (r%n)* n;
					//puzzle pos = xoffset to current position + position in smaller section + yoffset to current position
					int puzzlePos = xoff + (i*dim + j) + yoff;
					puzzle[puzzlePos] = s[n*i+j];
					//printf("%d ", s[n*i+j]);
				}
			}
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);


	if(rank == 0){
		printf("\n\n\n\n");
		printPuzzle(puzzle);
	}

	free(section);
	free(puzzle);
	MPI_Finalize();
}
