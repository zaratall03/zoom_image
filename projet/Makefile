lib-include-path := -I./include -Llib
linkage-opt := -lmingw32 -mwindows
ext-dependencies := $(lib-include-path) $(linkage-opt) -lSDL2main -lSDL2 
compiled-dir := ./exe
compiled-objects-dir:= $(compiled-dir)/obj
result-file := ./results
objects-dir-list := $(compiled-objects-dir)/window_fnct.o
objects-src-dir := ./src/gui/obj

needed-target = window_fnct

all: window

window: ./src/gui/main.c $(needed-target)
	@echo "Compilation of programme in GUI..."
	gcc $< $(objects-dir-list) $(ext-dependencies) -o $(compiled-dir)/$@

window_fnct :
	@echo "Compilation of graphical functions needed..."
	gcc -c $(objects-src-dir)/main_window.c $(ext-dependencies) -o $(compiled-objects-dir)/$@.o

GUI: window
	@echo "Launch main program in GUI..."

engine: ./src/engine/test_main_stb.c
	gcc -DSTB_IMAGE_IMPLEMENTATION $(lib-include-path) $< -o $(compiled-dir)/$@

exe-GUI : 
	$(compiled-dir)/window
	
exe-engine : 
	$(compiled-dir)/engine >  $(result-file)/engine.txt

image : ./src/engine/obj/image.c
	gcc -DSTB_IMAGE_IMPLEMENTATION $(lib-include-path) $(linkage-opt) $< -o $(compiled-objects-dir)/$@

exe-image :
	$(compiled-objects-dir)/image

clean : 
	rm $(compiled-dir)/*.exe
	rm $(compiled-objects-dir)/*.o
	rm $(result-file)/*.*