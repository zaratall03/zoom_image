img : 
	gcc -c src/engine/image.c -Ilib -o bin/obj/img.o

main : 
	gcc src/test.c bin/obj/img.o bin/obj/gui_fcnt.o bin/obj/interpolation.o -Ilib -o bin/test `pkg-config --cflags --libs gtk+-3.0` -lm -Wl,--gc-sections

inter :
	gcc -c src/engine/interpolation.c -Ilib  -o bin/obj/interpolation.o

gui_fnct : 
	gcc -c src/engine/gui_fcnt.c -Ilib -o bin/obj/gui_fcnt.o `pkg-config --cflags --libs gtk+-3.0` -lm



exe : main
	./bin/test



