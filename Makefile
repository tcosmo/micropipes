CC=gcc
LIBS=-lallegro -lallegro_primitives -lallegro_font -lallegro_image -lallegro_ttf -lallegro_dialog
FILES=src/*.c
INC=$(shell pwd)

main: $(FILES)
	$(CC) -o micropipes.out $(FILES) $(LIBS) $(LIBS_ALLEGRO) -I ${INC}