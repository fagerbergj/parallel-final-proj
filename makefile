CC = mpicc
LDC = mpicc
LD_FLAGS = -std=c11 -fopenmp
FLAGS = -std=c11 -fopenmp
RM= /bin/rm

CPROGS = sudoku_solver.cx

OBJSC=$(CPROGS:%.cx=%C.o) 

#all rule
all: $(CPROGS)

#$(CPROGS):$(OBJSC)
#	$(LDC) $(LD_FLAGS) $@ -o $@

sudoku_solver.cx:sudoku_solverC.o
	$(LDC) $(LD_FLAGS) $< -o $@

%C.o: %.c
	$(CC) $(FLAGS) -c $^ -o $@



#clean rule
clean:
	$(RM) -rf *.o *.x *.cx *.mod

