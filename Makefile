.PHONY: all run clean

PROGRAM_NAME = dsp-sqlite
SOURCES_DSP_SQLITE = $(wildcard *.c)
SOURCES_LIB_PSAFE = $(wildcard psafe/*.c)
SOURCES = $(SOURCES_LIB_PSAFE) $(SOURCES_DSP_SQLITE)

CC = gcc
LINK_LIBS = -lsqlite3
CFLAGS = -g

$(PROGRAM_NAME): $(SOURCES)
	@$(CC) $(CFLAGS) $^ -o $(PROGRAM_NAME) $(LINK_LIBS)

all: $(PROGRAM_NAME)

run: $(PROGRAM_NAME)
	@rm -rf *.db
	@./$(PROGRAM_NAME)

memtest: $(PROGRAM_NAME)
	@rm -rf *.db
	@valgrind ./$(PROGRAM_NAME)

clean:
	@rm -rf $(PROGRAM_NAME) *.db *.o *.out *.out.* vgcore.*
