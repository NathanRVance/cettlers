settlersbin:
	gcc -ggdb -o cettlers *.c *.h -lncurses -lm
clean:
	rm -f settlers
