CC = gcc
LDC = gcc
LD_FLAGS = -std=c11 -fopenmp -lm
FLAGS = -std=c11 -fopenmp -lm
RM= /bin/rm

CPROGS = sudoku.cx

OBJSC=$(CPROGS:%.cx=%.o) 

#all rule
all: $(CPROGS)

$(CPROGS):$(OBJSC)
	$(LDC) $(LD_FLAGS) $< -o $@

#sudoku_solver.cx:sudoku_solverC.o
#	$(LDC) $(LD_FLAGS) $< -o $@

%.o: %.c
	$(CC) $(FLAGS) -c $^ -o $@



#clean rule
clean:
	$(RM) -rf *.o *.x *.cx *.mod

