BIN=lost_cities

${BIN}: game.c game.h card_stack.h card_stack.c main.c
	gcc card_stack.c game.c main.c -o ${BIN}

test: game.c game.h test.c card_stack.h card_stack.c
	gcc card_stack.c game.c test.c -o ${BIN}_test

debug: game.c game.h test.c card_stack.h card_stack.c
	gcc -g card_stack.c game.c test.c -o ${BIN}_debug

clean:
	@rm -f *.o ${BIN} ${BIN}_test ${BIN}_debug
	@rm -rf *.dSYM

