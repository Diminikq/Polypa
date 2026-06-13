##########################
# Author: Dominik Makuka
# VUT FIT Brno


CC = clang 
CFLAGS = -Wall -Werror -Wextra -pedantic -std=c11

.PHONY: clean

polypa: polypa.o polynom.o horner.o divisors.o roots.o polypa_config.o
	$(CC) $(CFLAGS) $^ -o $@

polypa.o : polypa.c polynom.h horner.h
	$(CC) $(CFLAGS) -c $<

polynom.o: polynom.c polynom.h
	$(CC) $(CFLAGS) -c $<	

horner.o: horner.c horner.h polynom.h divisors.h roots.h
	$(CC) $(CFLAGS) -c $<

divisors.o: divisors.c divisors.h
	$(CC) $(CFLAGS) -c $<

roots.o: roots.c roots.h polynom.h horner.h
	$(CC) $(CFLAGS) -c $<

polypa_config.o: polypa_config.c polypa_config.h polynom.h horner.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o polypa