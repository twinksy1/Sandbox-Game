FILENAME=main
FLAGS=-lSDL2 -lSDL2_ttf

all: $(FILENAME)

$(FILENAME): $(FILENAME).cpp
	g++ $< -o $@ $(FLAGS)

clean:
	rm -f $(FILENAME)
