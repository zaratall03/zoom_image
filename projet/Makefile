# lib-include-path := -I./include -Llib
# linkage-opt := -lmingw32 -mwindows
# ext-dependencies := $(lib-include-path) $(linkage-opt) -lSDL2main -lSDL2 
# compiled-dir := ./exe
# compiled-objects-dir:= $(compiled-dir)/obj
# result-file := ./results



# all: engine gui

# # engine gui 	
# gui: ./src/gui/main.c $(compiled-objects-dir)/window_fnct.o $(compiled-objects-dir)/painter.o $(compiled-objects-dir)/image.o $(compiled-objects-dir)/interpolation.o
# 	@echo "Compilation of program in GUI..."
# 	gcc $^ $(ext-dependencies) -o $(compiled-dir)/$@

# window_fnct : ./src/gui/obj/window_fnct.c 
# 	@echo "Compilation of graphical functions needed..."
# 	gcc -c $< $(ext-dependencies) -o $(compiled-objects-dir)/$@.o


# engine : src/engine/test_main_stb.c exe/obj/image.o exe/obj/interpolation.o
# 	gcc -DSTB_IMAGE_IMPLEMENTATION $^ $(lib-include-path) -o $(compiled-dir)/$@

# painter : ./src/gui/obj/painter.c
# 	gcc -c $^ $(lib-include-path) -o $(compiled-objects-dir)/$@.o

# image : ./src/engine/obj/image.c 
# 	gcc -c $< -DSTB_IMAGE_IMPLEMENTATION $(ext-dependencies) -o $(compiled-objects-dir)/$@.o

# interpolation : ./src/gui/obj/interpolation.c
# 	gcc -c $< $(linkage-opt) $(lib-include-path) -lm -o $(compiled-objects-dir)/$@.o

# test : ./src/engine/test_inte.c $(compiled-objects-dir)/window_fnct.o $(compiled-objects-dir)/painter.o $(compiled-objects-dir)/image.o $(compiled-objects-dir)/interpolation.o
# 	gcc -DSTB_IMAGE_IMPLEMENTATION $^ $(lib-include-path) -o $(compiled-dir)/$@

# wind : ./src/gui/test_wind.c
# 	gcc $^ $(ext-dependencies) -o $(compiled-dir)/$@

# exe-wind : 
# 	./$(compiled-dir)/wind

# # Execution
# compile-engine : image interpolation engine

# compile-gui : window_fnct painter image interpolation gui

# compile-test : window_fnct painter image interpolation test

# exe-gui : 
# 	$(compiled-dir)/GUI

# exe-engine : 
# 	$(compiled-dir)/engine > ./results/res.txt

# clean : 
# 	rm $(compiled-dir)/*.exe
# 	rm $(compiled-objects-dir)/*.o
# 	rm $(result-file)/*.*

img : 
	gcc -c src/engine/image.c -Ilib -o bin/obj/img.o

main : 
	gcc src/test.c bin/obj/img.o -Ilib -o bin/test -I/path/to/stb_image `pkg-config --cflags --libs gtk+-3.0` -lm



exe : 
	./bin/test


