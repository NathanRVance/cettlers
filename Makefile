settlersbin:
	gcc -ggdb -o settlers *.c *.h -lncurses -lm
clean:
	rm -f settlers
