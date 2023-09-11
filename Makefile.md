# CC=gcc
# CFLAGS=-I.

# client: client.c requests.c helpers.c buffer.c
# 	$(CC) -o client client.c requests.c helpers.c buffer.c -Wall

# run: client
# 	./client

# clean:
# 	rm -f *.o client

CC=gcc
CFLAGS=-I.

# Adăugați -ljson-c la opțiunea LDLIBS pentru a lega biblioteca json-c
LDLIBS=-ljson-c

client: client.c requests.c helpers.c buffer.c
	$(CC) -o client client.c requests.c helpers.c buffer.c -Wall $(LDLIBS)

run: client
	./client

clean:
	rm -f *.o client
