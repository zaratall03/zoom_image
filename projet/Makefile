ext-dependencies := -I./include -Llib -lmingw32 -lSDL2main -lSDL2 -mwindows
compiled-dir := ./exe
compiled-objects-dir:= $(compiled-dir)/obj

objects-dir-list := $(compiled-objects-dir)/window_fnct.o
objects-src-dir := ./src/gui/obj

needed-target = window_fnct

all: window

window: ./src/gui/main.c $(needed-target)
	@echo "Compilation of programme in GUI..."
	gcc ./src/gui/main.c $(objects-dir-list) $(ext-dependencies) -o $(compiled-dir)/$@

window_fnct :
	@echo "Compilation of graphical functions needed..."
	gcc -c $(objects-src-dir)/main_window.c $(ext-dependencies) -o $(compiled-objects-dir)/$@.o

GUI: needed_folder window
	@echo "Launch main program in GUI..."
	$(compiled-dir)/window

needed_folder:
	echo "Creation of executable folder...";
	mkdir "$(compiled-dir)";
	echo "Creation of objects folder...";
	mkdir "$(compiled-objects-dir)";

clean : 
	rm -r $(compiled-dir)