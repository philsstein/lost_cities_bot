BIN=lost_cities

game: game.c game.h test.c
	gcc game.c test.c -o ${BIN}

clean:
	@rm -f *.o ${BIN}

