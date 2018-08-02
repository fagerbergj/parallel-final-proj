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

int validMove(int* puzzle, int x, int y, int num){
	int rowStart = (x/n*n) * n*n;
	int colStart = (y/n*n) * n*n;

	for(int i=0; i<n*n; ++i){
		if (puzzle[dim*x + i] == num) return 0;
		if (puzzle[dim*i + y] == num) return 0;
		if (puzzle[dim*(rowStart + (i%n*n)) + (colStart + (i/n*n))] == num) return 0;
	}
	return 1;
}

int* genPuzzle(){
	//set random seed
	srand(time(NULL));

	//all 0s
	int* puzzle = (int*)malloc(totalSize*sizeof(int));
	for(int i = 0; i < totalSize; i++){
		puzzle[i] = 0;
	}

	//n*n random spots
	for(int i = 1; i <= n*n; i++){
		int randPos = rand() % totalSize;
		int x = getX(randPos);
		int y = getY(randPos);

		while(!validMove(puzzle, x, y, i)){
			x = getX(randPos);
                	y = getY(randPos);
			randPos = rand() % totalSize;
		}
		puzzle[randPos] = i;
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
	n = 2;
	//number of row/col in puzzle
	dim = n*n;
	//total number of elements in puzzle dim*dim
	totalSize = dim*dim;

	if(rank == 0){
		int* puzzle = (int*)malloc(totalSize*sizeof(int));
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
	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Recv(section, dim, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	for(int i = 0; i < numranks; i++){
		if(rank == i){
			printf("*****RANK: %d's SECTION*****\n", rank);
			for(int i = 0; i < n; i++){
				for(int j = 0; j < n; j++){
					printf("%d ", section[i*n+j]);
				}
				printf("\n");
			}
		}
	}
	//now start solving each qn

	// for 0 -> n*n - 1
		// for each rank: r
			//rank r then places a number into first available spot
				//rank r checks to see if number is in Qr
				//rank r askes all ranks horizontal if they have that num in the selected row
				//rank r askes all ranks vertical if they have that num in the selected column
			//if valid move, go to next rank
			//if invalid move pick another number

	MPI_Finalize();
}
