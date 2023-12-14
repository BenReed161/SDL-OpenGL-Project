TARG = main
BINARY = main
LDFLAGS = -lGL -lGLEW -lGLU -lglut -lX11 -lSDL2main

make:
	gcc -o $(BINARY) src/$(TARG).c $(LDFLAGS) $$(pkg-config --cflags --libs sdl2)
cpp:
	g++ -o $(BINARY) src/$(TARG).cpp $(LDFLAGS) $$(pkg-config --cflags --libs sdl2)
clean:
	rm -f $(BINARY)

#  -lglut -lGL