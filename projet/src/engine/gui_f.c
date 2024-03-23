#include "callback.h"
#include "gui_f.h"
#include <gtk/gtk.h>


void initMainWindow(GtkWidget *window) {
    if (!GTK_IS_WIDGET(window)) {
        printf("Erreur : le widget n'est pas une fenêtre GTK.\n");
        return;
    }
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_title(GTK_WINDOW(window), "Comparateur de Zoom");
}


