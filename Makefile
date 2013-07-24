# ugly ugly ugly :(
CFLAGS := -march=native -std=c99 -Os -pipe -Wall -Wextra -Werror -pedantic \
          `pkg-config --cflags sqlite3` `pkg-config --cflags yajl` \
          `pkg-config --cflags libcurl`
LDFLAGS := `pkg-config --libs sqlite3` `pkg-config --libs yajl` \
           `pkg-config --libs libcurl`
SOURCES := $(patsubst %.c,%.o,$(wildcard src/*.c))
.PHONY : clean
all: $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o poniko3 $(SOURCES)
clean:
	rm -f poniko3
	rm -f $(SOURCES)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<