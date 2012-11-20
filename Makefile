include /home/users/cse534/Stevens/unpv13e/Make.defines
# This is a sample Makefile which compiles source files named:
# - tcpechotimeserv.c
# - tcpechotimecliv.c
# - time_cli.c
# - echo_cli.c
# and creating executables: "server", "client", "time_cli"
# and "echo_cli", respectively.
#
# It uses various standard libraries, and the copy of Stevens'
# library "libunp.a" in ~cse533/Stevens/unpv13e_solaris2.10 .
#
# It also picks up the thread-safe version of "readline.c"
# from Stevens' directory "threads" and uses it when building
# the executable "server".
#
# It is set up, for illustrative purposes, to enable you to use
# the Stevens code in the ~cse533/Stevens/unpv13e_solaris2.10/lib
# subdirectory (where, for example, the file "unp.h" is located)
# without your needing to maintain your own, local copies of that
# code, and without your needing to include such code in the
# submissions of your assignments.
#
# Modify it as needed, and include it with your submission.

CC = gcc

LIBS = /home/users/cse533/Stevens/unpv13e/libunp.a -lpthread\

FLAGS = -g -O2

CFLAGS = ${FLAGS} -I/home/users/cse533/Stevens/unpv13e/lib


all: client server odr

client: client.o
	${CC} ${FLAGS} -o client client.o ${LIBS}
client.o: client.c
	${CC} ${CFLAGS} -c client.c


server: server.o
	${CC} ${FLAGS} -o server server.o ${LIBS}
server.o: server.c
	${CC} ${CFLAGS} -c server.c


odr: odr.o
	${CC} ${FLAGS} -o odr odr.o ${LIBS}
odr.o: odr.c
	${CC} ${CFLAGS} -c odr.c

# pick up the thread-safe version of readline.c from directory "threads"


clean:
	rm client server odr
