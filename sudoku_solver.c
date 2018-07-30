#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "omp.h"
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
	int i, j;

	for(i=0; i<n*n; ++i){
		if (puzzle[numRows*x + i] == num) return 0;
		if (puzzle[numRows*i + y] == num) return 0;
		if (puzzle[numRows*(rowStart + (i%n*n)) + (colStart + (i/n*n))] == num) return 0;
	}
	return 1;
}

//TODO put numbers 1->n in valid random locations
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

	//size of inner squares n*n
	n = 3;
	//number of rows in puzzle
	numRows = n*n;
	//number of colums in puzzle
	numCols = n*n;
	//total number of elements in puzzle numRows*numCols
	totalSize = n*n*n*n;

	int* puzzle = (int*)malloc(totalSize*sizeof(int));
	puzzle = genPuzzle();

	printPuzzle(puzzle);
}
