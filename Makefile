TARG = main
BINARY = main
LDFLAGS = -lGL -lGLEW -lGLU -lX11 -lSDL2main

make:
	gcc -o $(BINARY) src/$(TARG).c $(LDFLAGS) $$(pkg-config --cflags --libs sdl2)
clean:
	rm -f $(BINARY)

#  -lglut -lGL