CC      = mpicc
CFLAGS  = -O2 -Wall
TARGET  = poisson

all: $(TARGET)

$(TARGET): poisson_solver.c
	$(CC) $(CFLAGS) -o $@ $< -lm

run: $(TARGET)
	mpirun -n 4 ./$(TARGET) 100 100

clean:
	rm -f $(TARGET)
