CC = gcc
LDC = gcc
LD_FLAGS = -lm -fopenmp
FLAGS = -fopenmp
PROGC = sudoku.cx
RM= /bin/rm


#all rule

OBJSC=$(PROGC:%.cx=%.o)

all: $(PROGC)

$(PROGC): $(OBJSC)
	$(LDC) $< $(LD_FLAGS) -o $@

%.o: %.c
	$(CC) $(FLAGS) -c $^ -o $@

#clean rule
clean:
	$(RM) -rf *.o *.x *.cx *.mod

