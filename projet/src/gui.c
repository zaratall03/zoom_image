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

GdkPixbuf *pixbufImg;
Image img;

ZoomType TYPE_ALGO;
void on_type_algo_changed(GtkComboBox *combo_box, gpointer user_data);
void init_type_algo_combo(GtkComboBox *combo_box);

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
    GtkComboBox *typeAlgoCombo;
    Image originalImg;

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, GLADE_FILE, NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    imageEventBox = GTK_WIDGET(gtk_builder_get_object(builder, "eventbox1"));  // Récupérer la GtkEventBox
    image = GTK_WIDGET(gtk_builder_get_object(builder, "image1"));
    zoomInButton = GTK_WIDGET(gtk_builder_get_object(builder, "zoomIn"));
    zoomOutButton = GTK_WIDGET(gtk_builder_get_object(builder, "ZoomOut"));
    openOption = GTK_WIDGET(gtk_builder_get_object(builder, "OpenOption"));
    typeAlgoCombo = GTK_COMBO_BOX(gtk_builder_get_object(builder, "TypeAlgo"));

    initMainWindow(window);
    init_type_algo_combo(typeAlgoCombo);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(openOption, "activate", G_CALLBACK(open_file), image);
    g_signal_connect(zoomInButton, "clicked", G_CALLBACK(on_zoom_in_clicked), zoomOutButton);
    g_signal_connect(zoomOutButton, "clicked", G_CALLBACK(on_zoom_out_clicked), zoomInButton);
    g_signal_connect(imageEventBox, "button-release-event", G_CALLBACK(on_mouse_button_release), image);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

void on_type_algo_changed(GtkComboBox *combo_box, gpointer user_data) {
    GtkTreeIter iter;
    if (gtk_combo_box_get_active_iter(combo_box, &iter)) {
        gchar *item = NULL;
        gtk_tree_model_get(gtk_combo_box_get_model(combo_box), &iter, 0, &item, -1);
        if (g_strcmp0(item, "BILINEAR") == 0) {
            TYPE_ALGO = BILINEAR;
        } else if (g_strcmp0(item, "HERMITE") == 0) {
            TYPE_ALGO = HERMITE;
        } else if (g_strcmp0(item, "NEAREST NEIGHBOR") == 0) {
            TYPE_ALGO = NEAREST_NEIGHBOR;
        }
        g_free(item);
    }
}


void init_type_algo_combo(GtkComboBox *combo_box) {
    combo_box = GTK_COMBO_BOX(gtk_combo_box_text_new());
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "BILINEAR");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "HERMITE");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "NEAREST NEIGHBOR");
    g_signal_connect(combo_box, "changed", G_CALLBACK(on_type_algo_changed), NULL);
}
