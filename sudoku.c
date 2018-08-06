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
	for(int i = 1; i <= dim; i++){
		int randPos = rand() % totalSize;
		int x = getX(randPos);
		int y = getY(randPos);

		while(!validMove(x, y, i)){
			x = getX(randPos);
                        y = getY(randPos);
			randPos = rand() % totalSize;
		}
		puzzle[randPos] = i;
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

int main(int argc, char **argv)
{
	//size of inner squares n*n
	/*#pragma omp parallel
	{
		n = (int)sqrt(omp_get_num_threads());
	}*/
        n=3;
	//number of row/col in puzzle
	dim = n*n;
	//total number of elements in puzzle dim*dim
	totalSize = dim*dim;

	puzzle = (int*)malloc(totalSize*sizeof(int));
	genPuzzle();

	printPuzzle();

	free(puzzle); 
}
