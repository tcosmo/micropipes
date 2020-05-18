CC=gcc
LIBS=-lallegro -lallegro_image
FILES=src/*.c
INC=$(shell pwd)

main: $(FILES)
	$(CC) -o micropipes.out $(FILES) $(LIBS) $(LIBS_ALLEGRO) -I ${INC}