OBJ=blocks.o main.o
EXE=app
HEADERS=*.h
LIBS=-lSDL2 -lSDL2_ttf -lSDL2_image -lm
ARGS=-c
CC=g++

all: $(EXE)

%.o: %.cpp $(HEADERS)
	$(CC) $(ARGS) $< -o $@

$(EXE): $(OBJ)
	$(CC) $^ -o $@ $(LIBS)

clean:
	rm $(EXE) *.o
