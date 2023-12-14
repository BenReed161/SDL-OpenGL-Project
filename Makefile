TARG = main
BINARY = main
LDFLAGS = -lGL -lGLEW -lGLU -lglut -lX11 -lSDL2main

make:
	g++ -o $(BINARY) src/$(TARG).cpp $(LDFLAGS) $$(pkg-config --cflags --libs sdl2)
clean:
	rm -f $(BINARY)

#  -lglut -lGL