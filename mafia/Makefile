CC = clang++
CFLAGS = -O2 -Wall -std=c++20 -pthread
TRASH = *.out
GENS = mafia

mafia: main.cpp GameMaster.cpp roles.cpp Share_ptr.cpp
	$(CC) $(CFLAGS) main.cpp GameMaster.cpp roles.cpp Share_ptr.cpp -o mafia

clean:
	rm -f $(TRASH)

distclean:
	rm -rf $(GENS)

run: mafia
	./mafia
