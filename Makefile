.PHONY: all clean

all:
	@gcc -g *.c -o dsp-sqlite -lsqlite3

run:
	@./dsp-sqlite

clean:
	@rm -rf dsp-sqlite
