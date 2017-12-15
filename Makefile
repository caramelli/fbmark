PREFIX ?= /usr/local

CFLAGS += -g -O2

PROGRAMS = fb_mandelbrot fb_rectangle fb_sierpinski

all: $(PROGRAMS)

fb_mandelbrot: fb_mandelbrot.c
fb_rectangle: fb_rectangle.c
fb_mandelbrot fb_rectangle:
	$(CC) -Wall $^ -o $@ $(CFLAGS) $(LDFLAGS)

fb_sierpinski: fb_sierpinski.c
	$(CC) -Wall $^ -o $@ -lm $(CFLAGS) $(LDFLAGS)

clean:
	$(RM) $(PROGRAMS)

install: all
	install -d $(DESTDIR)$(PREFIX)/bin
	install $(PROGRAMS) $(DESTDIR)$(PREFIX)/bin
