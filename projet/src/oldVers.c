#include "callback.h"

// Déclaration des variables globales
Image imageSrc; // Variable globale pour stocker l'image originale
Image zoomedImage; // Variable globale pour stocker l'image zoomée
GtkWidget *image; // Variable globale pour le widget GtkImage
MouseCallbackData *callbackData;
GtkWidget *window; 


int main(int argc, char *argv[]) {
    // Initialisation de GTK+
    gtk_init(&argc, &argv);
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);//
    //OK
    GtkWidget *file_chooser_button = gtk_file_chooser_button_new("Sélectionner une image", GTK_FILE_CHOOSER_ACTION_OPEN);
    image = gtk_image_new();
    callbackData = g_new(MouseCallbackData, 1);
    GtkWidget *combobox = gtk_combo_box_text_new();
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // Création de la fenêtre principale
    gtk_window_set_title(GTK_WINDOW(window), "Affichage d'une image");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_widget_set_size_request(window, 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Création du GtkFileChooserButton pour sélectionner l'image
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_chooser_button), g_get_home_dir());
    gtk_widget_set_hexpand(file_chooser_button, TRUE);

    // Création du GtkImage pour afficher l'image sélectionnée
    gtk_widget_set_hexpand(image, TRUE);
    gtk_widget_set_vexpand(image, TRUE);

    // Connecter la fonction de rappel pour mettre à jour l'image lors de la sélection du fichier
    g_signal_connect(file_chooser_button, "file-set", G_CALLBACK(on_file_selection), NULL);



//OKAY
    // Création de la combobox pour sélectionner le type de zoom
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "BILINEAR");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "HERMITE");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "NEAREST NEIGHBOR");
    g_signal_connect(combobox, "changed", G_CALLBACK(on_combobox_changed), NULL);

    // Création d'une structure MouseCallbackData et initialisation du type de zoom

    // Connecter la fonction de rappel pour détecter les clics de souris sur la fenêtre principale
    g_signal_connect(window, "button-press-event", G_CALLBACK(on_mouse_button_press), callbackData);

    // Création d'une boîte verticale pour organiser les widgets
    gtk_box_pack_start(GTK_BOX(vbox), file_chooser_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), combobox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), image, TRUE, TRUE, 0);

    // Ajout de la boîte verticale à la fenêtre principale
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Affichage de la fenêtre
    gtk_widget_show_all(window);

    // Lancement de la boucle principale GTK+
    gtk_main();

    // Libérer la mémoire allouée pour la structure MouseCallbackData
    g_free(callbackData);

    return 0;
}
