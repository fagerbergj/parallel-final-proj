#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <math.h>

int totalSize;
int dim;
int n;
int* puzzle;

int getX(int pos){
	return pos/dim;
}

int getY(int pos){
	return pos%dim;
}

int hasUnassigned(){
	int ret = 0;
	#pragma omp parallel for
	for(int i = 0; i < totalSize; i++){
		if(puzzle[i] == 0) ret = 1;
	}
	return ret;
}

int * getSecBounds(int x, int y)
{
	int * sec = (int*)malloc(4*sizeof(int));
	for(int i=0;i<dim; i+=n)
	{
		if(i<=x&&x<i+n)
		{
                    //printf("X) %d<=%d && %d<%d\n",i,x,x,i+n);
			sec[0]=i;
			sec[1]=i+n-1;
		}
		if(i<=y&&y<i+n)
		{
                    //printf("Y) %d<=%d && %d<%d\n",i,y,y,i+n);
			sec[2]=i;
			sec[3]=i+n-1;
		}
	}
	return sec;
}

int validMove(int x, int y, int k)
{
	int check = 1;
	//Check Row/Column
	#pragma omp parallel for
	for(int i=0;i<dim;i++)
        {
	    if(i!=x&&puzzle[dim*i+y]==k)
	        check=0;
	    if(i!=y&&puzzle[dim*x+i]==k)
	        check=0;
        }

	//Check Sections
        //printf("X= %d, Y= %d\n",x,y);
	int * sec = getSecBounds(x, y);
        //for(int i=0;i<4;i++)
        //    printf("sec[%d] = %d\n",i,sec[i]);
        //printf("\n");

	#pragma omp parallel for
	for(int i=sec[0];i<sec[1];i++)
		for(int j=sec[2];j<sec[3];j++)
			if(i!=x&&j!=y&&puzzle[dim*i+j]==k)
				check=0;
        free(sec);
	return check;
}

void genPuzzle(){
	//set random seed
	srand(time(NULL));

	//all 0s
	#pragma omp parallel for
	for(int i = 0; i < totalSize; i++){
		puzzle[i] = 0;
	}

	//n*n random spots
	for(int i = 0; i <= (int)((double)(totalSize) * .35); i++){
		int randPos = rand() % totalSize;
		int randNum = rand() % dim;
		int x = getX(randPos);
		int y = getY(randPos);

		while(puzzle[randPos] !=0 || !validMove(x, y, randNum)){
			randPos = rand() % totalSize;
			x = getX(randPos);
                        y = getY(randPos);
			randNum = rand() % dim;
		}
		puzzle[randPos] = randNum;
	}
}

void printPuzzle(){
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

void solvePuzzle(){
	for(int i = 0; i < totalSize; i++){
		int num = rand() % dim;
		for(int noff = 0; noff < dim; noff++){
			num = ((num + noff) % dim) + 1;
			if(puzzle[i] == 0 && validMove(getX(i), getY(i), num)){
				puzzle[i] = num;
				break;
			}
		}
	}
}

int main(int argc, char **argv)
{
	//size of inner squares n*n
	/*#pragma omp parallel
	{
		n = (int)sqrt(omp_get_num_threads());
	}*/
        n=10;
	//number of row/col in puzzle
	dim = n*n;
	//total number of elements in puzzle dim*dim
	totalSize = dim*dim;

	int nthreads;
	double t1;
	//#pragma omp parallel{
	nthreads = omp_get_num_threads();
	t1 = omp_get_wtime();
	//}

	puzzle = (int*)malloc(totalSize*sizeof(int));
	genPuzzle();

	//printPuzzle();

	solvePuzzle();

	printf("\n\n\n");
	//printPuzzle();

	if(hasUnassigned()){
		printf("SOLVER FAILED TO FIND SOLUTION\n");
	}

	double t2;
	//#pragma omp parallel{
	t2 = omp_get_wtime();
	//}

	printf("numthreads: %d\ntime: %.4f\n\n", nthreads, t2-t1);


	free(puzzle); 
}

