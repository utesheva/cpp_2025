CC = clang++
CFLAGS = -O2 -std=c++20 -pthread
GENS = SA 1_experiment 2_experiment

all: SA e1 e2

SA: main.cpp
	$(CC) $(CFLAGS) main.cpp -o SA

e1: main_1_exp.cpp
	$(CC) $(CFLAGS) main_1_exp.cpp -o 1_experiment

e2: main_2_exp.cpp
	$(CC) $(CFLAGS) main_2_exp.cpp -o 2_experiment

distclean:
	rm -rf $(GENS)

run_sa: SA
	./SA

run_e1: e1
	./1_experiment

run_e2: e2
	./2_experiment
