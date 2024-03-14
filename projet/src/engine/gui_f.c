#include "callback.h"
#include "gui_f.h"
#include <gtk/gtk.h>

extern Image imageSrc; // Variable globale pour stocker l'image originale
extern Image zoomedImage; // Variable globale pour stocker l'image zoomée
extern GtkWidget *image; // Variable globale pour le widget GtkImage
extern MouseCallbackData *callbackData;
extern GtkWidget *window; 
extern GtkWidget *file_chooser_button;
extern GtkWidget *combobox; 
extern GtkWidget *vbox;

void init_main_window() {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Affichage d'une image");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_widget_set_size_request(window, 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}

void init_image_chooser() {
    file_chooser_button = gtk_file_chooser_button_new("Sélectionner une image", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_chooser_button), g_get_home_dir());
    gtk_widget_set_hexpand(file_chooser_button, TRUE);
    g_signal_connect(file_chooser_button, "file-set", G_CALLBACK(on_file_selection), NULL);
}

void init_image_displayer() {
    image = gtk_image_new();
    gtk_widget_set_hexpand(image, TRUE);
    gtk_widget_set_vexpand(image, TRUE);
}


void init_zoom_type_chooser() {
    callbackData = g_new(MouseCallbackData, 1);
    combobox = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "BILINEAR");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "HERMITE");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "NEAREST NEIGHBOR");
    g_signal_connect(combobox, "changed", G_CALLBACK(on_combobox_changed), NULL);
    g_signal_connect(window, "button-press-event", G_CALLBACK(on_mouse_button_press), callbackData);
}

void pack_all() {
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), file_chooser_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), combobox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), image, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
}
