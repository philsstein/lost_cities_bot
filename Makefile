CC=gcc
CFLAGS=-Wall -g -std=c99 -D_XOPEN_SOURCE=700

BIN=lost_cities
OBJS=game.o card_stack.o irc_game.o
SRC=${OBJS:.o=.c}

LIB_IRC_PATH=../libircclient-1.7
INCLUDES=-I /usr/include -I ${LIB_IRC_PATH}/include
LIBS=${LIB_IRC_PATH}/src/libircclient.a

${BIN}: main.o ${OBJS}
	${CC} ${CFLAGS} ${INCLUDES} main.o ${OBJS} -o ${BIN} ${LIBS}

test: test.o ${OBJS}
	${CC} ${CFLAGS} ${INCLUDES} test.o ${OBJS} -o test ${LIBS}

game.o: game.h game.c
card_stack.o: card_stack.c card_stack.h
irc_game.o: irc_game.c irc_game.h

tags: main.c test.c ${SRC}
	ctags -R . ${LIB_IRC_PATH}/include

clean:
	@rm -f *.o ${BIN} test debug
	@rm -rf *.dSYM core*

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<
