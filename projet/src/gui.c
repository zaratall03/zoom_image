#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>
#include <time.h>

#include "interpolation.h"
#include "image.h"
#include "callback.h"
#include "gui_f.h"

#define GLADE_FILE "ressources/gui/gui.glade"


gboolean zoomInClicked = FALSE;
gboolean zoomOutClicked = FALSE;
ResultTab resultTab;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

ZoomType displayedZoomType;
short uploaded = 0;
ResultTab resultTab;

GtkWidget *labels[NB_TYPE];
GtkBuilder *builder;
GtkWidget *drawing_area;

char* algo_displayed[NB_TYPE] = {"PPV", "Bilinéaire", "Bicubique"};


int main(int argc, char *argv[]) {
    initializeResultTab(); // Initialisation du tableau de résultat
    gtk_init(&argc, &argv);

    GtkWidget *window;
    GtkWidget *imageEventBox;
    GtkWidget *image;
    GtkWidget *zoomInButton;
    GtkWidget *zoomOutButton;
    GtkWidget *openOption;
    GtkWidget *saveOption;
    GtkWidget *typeAlgoEntry;
    GtkWidget *algoSelector;
    GtkWidget *quitOption;

    Image originalImg;
    // Chargement du fichier Glade
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, GLADE_FILE, NULL);

    // Chargement des différents widgets sur lesquels on travaille
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    imageEventBox = GTK_WIDGET(gtk_builder_get_object(builder, "eventbox1"));
    image = GTK_WIDGET(gtk_builder_get_object(builder, "image1"));
    zoomInButton = GTK_WIDGET(gtk_builder_get_object(builder, "zoomIn"));
    zoomOutButton = GTK_WIDGET(gtk_builder_get_object(builder, "ZoomOut"));
    openOption = GTK_WIDGET(gtk_builder_get_object(builder, "OpenOption"));
    saveOption = GTK_WIDGET(gtk_builder_get_object(builder, "SaveOption"));
    algoSelector = GTK_WIDGET(gtk_builder_get_object(builder, "algoSelector"));
    drawing_area = GTK_WIDGET(gtk_builder_get_object(builder, "histo"));
    

    initMainWindow(window);
    // Connection des wigdet aux différents signaux pour lancer les fonctions de réactions
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(openOption, "activate", G_CALLBACK(open_file), image);
    g_signal_connect(saveOption, "activate", G_CALLBACK(save_file), image);
    g_signal_connect(zoomInButton, "clicked", G_CALLBACK(on_zoom_in_clicked), zoomOutButton);
    g_signal_connect(zoomOutButton, "clicked", G_CALLBACK(on_zoom_out_clicked), zoomInButton);
    g_signal_connect(imageEventBox, "button-release-event", G_CALLBACK(on_mouse_button_release), image);
    g_signal_connect(G_OBJECT(algoSelector), "changed", G_CALLBACK(on_combo_box_changed), image);
    g_signal_connect(G_OBJECT(algoSelector), "button-release-event", G_CALLBACK(on_combo_box_changed), image);
    g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(draw_histogram), image);
    
    

    gtk_widget_show_all(window);
    gtk_main();
    g_object_unref(G_OBJECT(builder));

    return 0;
}
