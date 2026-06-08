CC = clang 
CFLAGS = -Wall -Werror -Wextra -pedantic -std=c11

.PHONY: clean

polypa: polypa.o polynom.o horner.o
	$(CC) $(CFLAGS) $^ -o $@

polypa.o : polypa.c polynom.h horner.h
	$(CC) $(CFLAGS) -c $<

polynom.o: polynom.c polynom.h
	$(CC) $(CFLAGS) -c $<	

horner.o: horner.c horner.h polynom.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o polypa