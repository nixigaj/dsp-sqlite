.PHONY: all run clean

all:
	@gcc -g *.c -o dsp-sqlite -lsqlite3

run:
	@rm data.db
	@./dsp-sqlite

clean:
	@rm -rf dsp-sqlite 
