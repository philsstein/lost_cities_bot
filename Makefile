BIN=lost_cities
GAME_SRC=game.c game.h card_stack.h card_stack.c irc_game.h irc_game.c
CFILES=game.c card_stack.c irc_game.c
LIB_IRC_PATH=../libircclient/libircclient-1.7
LIB_IRC_INC=${LIB_IRC_PATH}/include
LIB_IRC_LIB=-L ${LIB_IRC_PATH}/src -l ircclient
DEBUG=-g

${BIN}: main.c ${GAME_SRC}
	gcc ${DEBUG} -I ${LIB_IRC_INC} ${CFILES} main.c -o ${BIN} ${LIB_IRC_LIB}

test: test.c ${GAME_SRC}
	gcc -g -i ${LIB_IRC_INC} ${CFILES} test.c -o ${BIN}_test ${LIB_IRC_LIB}

tags: main.c test.c ${GAME_SRC}
	ctags -R . ${LIB_IRC_INC}

clean:
	@rm -f *.o ${BIN} ${BIN}_test ${BIN}_debug
	@rm -rf *.dSYM

