# Variables
CC = gcc
CFLAGS = -Iinclude
LIBS = `pkg-config --cflags --libs gtk+-3.0` -lm
OBJDIR = bin/obj
BINDIR = bin

directories:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)


# Cibles principales
all : directories img inter callback graphic_f gui 

# Compilation des fichiers interpolation.c
inter :
	$(CC) -c src/engine/interpolation.c $(CFLAGS) -o $(OBJDIR)/interpolation.o


# Compilation des fichiers image.c
img : 
	$(CC) -c src/engine/image.c $(CFLAGS) -o $(OBJDIR)/img.o

# Compilation des fichiers callback.c
callback : 
	$(CC) -c src/engine/callback.c $(CFLAGS) -o $(OBJDIR)/callback.o $(LIBS)

# Compilation du fichier gui_f.c
graphic_f : 
	$(CC) -c src/engine/gui_f.c  $(CFLAGS) -o $(OBJDIR)/graphic_f.o $(LIBS)

# Compilation du fichier gui.c et liage avec les objets compilés
gui :
	$(CC) $(OBJDIR)/interpolation.o $(OBJDIR)/graphic_f.o $(OBJDIR)/img.o $(OBJDIR)/callback.o src/gui.c $(CFLAGS) -o $(BINDIR)/$@ $(LIBS)

test :
	$(CC)  src/test.c $(CFLAGS) -o $(BINDIR)/$@ $(LIBS)
# Lancer l'exécutable
exe : 
	./$(BINDIR)/gui

exetest : 
	./$(BINDIR)/test


# Nettoyer les fichiers objets et l'exécutable
clean:
	rm -rf $(BINDIR)/*
	mkdir -p $(OBJDIR)
