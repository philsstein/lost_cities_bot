CC=gcc
CFLAGS=-Wall -g -std=c99 -D_XOPEN_SOURCE=700

BIN=lost_cities
OBJS=game.o card_stack.o irc_game.o markup.o game_commands.o log.o
SRC=${OBJS:.o=.c}

LIB_IRC_PATH=../libircclient-1.7
INCLUDES=-I /usr/include -I ${LIB_IRC_PATH}/include
LIBS=${LIB_IRC_PATH}/src/libircclient.a

.PHONY: clean tags cleanest

${BIN}: main.o ${OBJS}
	${CC} ${CFLAGS} ${INCLUDES} main.o ${OBJS} -o ${BIN} ${LIBS}

test: test.o ${OBJS}
	${CC} ${CFLAGS} ${INCLUDES} test.o ${OBJS} -o test ${LIBS}

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

tags: main.c test.c ${SRC}
	ctags -R . ${LIB_IRC_PATH}/include

clean:
	@rm -f *.o ${BIN} test debug
	@rm -rf *.dSYM core*

cleanest:
	@rm -f irc_game.log tags
	@rm -f *.o ${BIN} test debug
	@rm -rf *.dSYM core*
