#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "omp.h"
#include <mpi.h>
#include <time.h>

int totalSize;
int numRows;
int numCols;
int n;

int getX(int pos){
	return pos/numRows;
}

int getY(int pos){
	return pos%numRows;
}

int validMove(int* puzzle, int x, int y, int num){
	int rowStart = (x/n*n) * n*n;
	int colStart = (y/n*n) * n*n;

	for(int i=0; i<n*n; ++i){
		if (puzzle[numRows*x + i] == num) return 0;
		if (puzzle[numRows*i + y] == num) return 0;
		if (puzzle[numRows*(rowStart + (i%n*n)) + (colStart + (i/n*n))] == num) return 0;
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
	for(int i = 0; i < numRows; i++){
		if(i%n == 0 && i !=0 && i != numRows-1) {
			for(int u = 0; u < numCols*2+n*2; u++) printf("_");
			printf("\n");
		}
		for(int j = 0; j < numCols; j++){
			if(j%n == 0 && j !=0 && j!= numCols-1) {
				printf(" | ");
			}
			printf("%d ", puzzle[numRows*i + j]);
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
	n = 3;
	//number of rows in puzzle
	numRows = n*n;
	//number of colums in puzzle
	numCols = n*n;
	//total number of elements in puzzle numRows*numCols
	totalSize = n*n*n*n;

	if(rank == 0){
		int* puzzle = (int*)malloc(totalSize*sizeof(int));
		puzzle = genPuzzle();
		//send Qn to rank n

		printPuzzle(puzzle);
	}

	//for 0 -> numranks
		//recieve qn

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
