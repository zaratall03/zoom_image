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
ResultTab resultTab;


pthread_mutex_t lock= PTHREAD_MUTEX_INITIALIZER;

ZoomType displayedZoomType = BILINEAR;
short uploaded = 0;
ResultTab resultTab;

GtkWidget *labels[NB_TYPE];
GtkBuilder *builder;
GtkWidget *histogramDrawingArea;

int main(int argc, char *argv[]) {
    initializeResultTab();
    gtk_init(&argc, &argv);
    srand(time(NULL));

    GtkWidget *window;
    GtkWidget *imageEventBox;
    GtkWidget *image;
    GtkWidget *zoomInButton;
    GtkWidget *zoomOutButton;
    GtkWidget *openOption;
    GtkWidget *typeAlgoEntry;
    GtkWidget *algoSelector;



    Image originalImg;

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, GLADE_FILE, NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    imageEventBox = GTK_WIDGET(gtk_builder_get_object(builder, "eventbox1"));
    image = GTK_WIDGET(gtk_builder_get_object(builder, "image1"));
    zoomInButton = GTK_WIDGET(gtk_builder_get_object(builder, "zoomIn"));
    zoomOutButton = GTK_WIDGET(gtk_builder_get_object(builder, "ZoomOut"));
    openOption = GTK_WIDGET(gtk_builder_get_object(builder, "OpenOption"));
    algoSelector = GTK_WIDGET(gtk_builder_get_object(builder, "algoSelector"));
    histogramDrawingArea = GTK_WIDGET(gtk_builder_get_object(builder, "histo"));
    initMainWindow(window);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(openOption, "activate", G_CALLBACK(open_file), image);
    g_signal_connect(zoomInButton, "clicked", G_CALLBACK(on_zoom_in_clicked), zoomOutButton);
    g_signal_connect(zoomOutButton, "clicked", G_CALLBACK(on_zoom_out_clicked), zoomInButton);
    g_signal_connect(imageEventBox, "button-release-event", G_CALLBACK(on_mouse_button_release), image);
    g_signal_connect(G_OBJECT(algoSelector), "changed", G_CALLBACK(on_combo_box_changed), image);


    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}





