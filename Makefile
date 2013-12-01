BIN=lost_cities
GAME_SRC=game.c game.h card_stack.h card_stack.c

${BIN}: main.c ${GAME_SRC}
	gcc card_stack.c game.c main.c -o ${BIN}

test: test.c ${GAME_SRC}
	gcc card_stack.c game.c test.c -o ${BIN}_test

debug: test.c ${GAME_SRC}
	gcc -g card_stack.c game.c test.c -o ${BIN}_debug

tags: main.c test.c ${GAME_SRC}
	ctags -R . 

clean:
	@rm -f *.o ${BIN} ${BIN}_test ${BIN}_debug
	@rm -rf *.dSYM

