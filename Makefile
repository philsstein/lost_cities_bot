CC=gcc
CFLAGS=-Wall -g

BIN=lost_cities
OBJS=game.o card_stack.o irc_game.o
LIB_IRC_PATH=../libircclient/libircclient-1.7
INCLUDES=-I ${LIB_IRC_PATH}/include
LIBS=-L ${LIB_IRC_PATH}/src -l ircclient

${BIN}: main.o ${OBJS}
	${CC} ${CFLAGS} ${INCLUDES} main.o ${OBJS} -o ${BIN} ${LIBS}

test: test.o ${OBJS}
	${CC} ${CFLAGS} ${INCLUDES} test.o ${OBJS} -o test ${LIBS}

game.o: game.h game.c
card_stack.o: card_stack.c card_stack.h
irc_game.o: irc_game.c irc_game.h

tags: main.c test.c ${GAME_SRC}
	ctags -R . ${LIB_IRC_PATH}/include

clean:
	@rm -f *.o ${BIN} test debug
	@rm -rf *.dSYM

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<
