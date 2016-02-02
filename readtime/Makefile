MAKE=gcc
OPT=-w -lpthread
EXEC=main
ITER=10000

all:
	$(MAKE) main.c $(OPT) -o $(EXEC)

run:
	sudo ./$(EXEC) $(ITER)

clean:
	rm ./$(EXEC)
