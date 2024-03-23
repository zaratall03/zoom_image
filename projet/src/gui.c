#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "interpolation.h"
#include "image.h"
#include "callback.h"
#include "gui_f.h"

#define GLADE_FILE "ressources/gui/gui.glade"

gboolean zoomInClicked = FALSE;
gboolean zoomOutClicked = FALSE;



int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    srand(time(NULL));

    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *imageEventBox;  
    GtkWidget *image;
    GtkWidget *zoomInButton;
    GtkWidget *zoomOutButton;
    GtkWidget *openOption;

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, GLADE_FILE, NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    imageEventBox = GTK_WIDGET(gtk_builder_get_object(builder, "eventbox1"));  // Récupérer la GtkEventBox
    image = GTK_WIDGET(gtk_builder_get_object(builder, "image1"));
    zoomInButton = GTK_WIDGET(gtk_builder_get_object(builder, "zoomIn"));
    zoomOutButton = GTK_WIDGET(gtk_builder_get_object(builder, "ZoomOut"));
    openOption = GTK_WIDGET(gtk_builder_get_object(builder, "OpenOption"));

    initMainWindow(window);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(openOption, "activate", G_CALLBACK(open_file), image);
    g_signal_connect(zoomInButton, "clicked", G_CALLBACK(on_zoom_in_clicked), zoomOutButton);
    g_signal_connect(zoomOutButton, "clicked", G_CALLBACK(on_zoom_out_clicked), zoomInButton);
    g_signal_connect(imageEventBox, "button-release-event", G_CALLBACK(on_mouse_button_release), image);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}




