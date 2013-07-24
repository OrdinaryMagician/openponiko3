# ugly ugly ugly :(
CFLAGS := -march=native -std=c99 -Os -pipe -Wall -Wextra -Werror -pedantic \
          `pkg-config --cflags sqlite3`
LDFLAGS := `pkg-config --libs sqlite3`
SOURCES := $(patsubst %.c,%.o,$(wildcard src/*.c))
.PHONY : clean
all: $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o migrate $(SOURCES)
clean:
	rm -f migrate
	rm -f $(SOURCES)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
