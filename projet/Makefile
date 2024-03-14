# Variables
CC = gcc
CFLAGS = -Iinclude
LIBS = `pkg-config --cflags --libs gtk+-3.0` -lm
OBJDIR = bin/obj
BINDIR = bin

# Cibles principales
all : directories img inter callback gui main

# Création des répertoires s'ils n'existent pas
directories:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

# Compilation des fichiers image.c
img : 
	$(CC) -c src/engine/image.c $(CFLAGS) -o $(OBJDIR)/img.o

# Compilation des fichiers interpolation.c
inter :
	$(CC) -c src/engine/interpolation.c $(CFLAGS) -o $(OBJDIR)/interpolation.o

# Compilation des fichiers callback.c
callback : 
	$(CC) -c src/engine/callback.c $(CFLAGS) -o $(OBJDIR)/callback.o $(LIBS)

# Compilation des fichiers gui_f.c
gui : 
	$(CC) -c src/engine/gui_f.c $(CFLAGS) -o $(OBJDIR)/gui_f.o $(LIBS)

# Compilation du fichier test.c et liage avec les objets compilés
main : 
	$(CC) src/test.c $(OBJDIR)/img.o $(OBJDIR)/gui_f.o $(OBJDIR)/interpolation.o $(OBJDIR)/callback.o $(CFLAGS) -o $(BINDIR)/test $(LIBS) -Wl,--gc-sections

# Lancer l'exécutable
exe : main
	./$(BINDIR)/test

# Nettoyer les fichiers objets et l'exécutable
clean:
	rm -rf $(OBJDIR) $(BINDIR)
