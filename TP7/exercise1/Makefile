CC      = mpicc
CFLAGS  = -O2 -Wall
TARGET  = game_of_life

all: $(TARGET)

$(TARGET): parallel_game_of_life.c
	$(CC) $(CFLAGS) -o $@ $<

run: $(TARGET)
	mpirun -n 4 ./$(TARGET) 20 20 20 10

clean:
	rm -f $(TARGET)
