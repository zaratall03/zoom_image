#include <gtk/gtk.h>
#include "image.h" // Assurez-vous d'avoir inclus votre fichier d'en-tête contenant les définitions de vos fonctions et de votre structure Image.

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MAX_CHANNELS 4


gboolean on_mouse_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    if (event->type == GDK_BUTTON_PRESS && event->button == 1) {
        gdouble x = event->x;
        gdouble y = event->y;

        GtkWidget *image = GTK_WIDGET(data);
        GtkAllocation allocation;
        gtk_widget_get_allocation(image, &allocation);

        int image_width = allocation.width;
        int image_height = allocation.height;

        if (x >= 0 && x < image_width && y >= 0 && y < image_height) {
            // Les coordonnées du clic de souris sont à l'intérieur de la zone de l'image
            g_print("Clic de souris à l'intérieur de l'image: (%f, %f)\n", x, y);
        } else {
            // Les coordonnées du clic de souris ne sont pas à l'intérieur de l'image
            // Trouver les coordonnées les plus proches à l'intérieur de l'image
            double nearest_x = CLAMP(x, 0, image_width - 1);
            double nearest_y = CLAMP(y, 0, image_height - 1);
            g_print("Clic de souris à l'extérieur de l'image. Coordonnées les plus proches : (%f, %f)\n", nearest_x, nearest_y);
        }
    }

    return FALSE;
}
// Fonction de rappel appelée lorsque le bouton est cliqué
void on_file_selection(GtkFileChooserButton *filechooserbutton, gpointer data) {
    const gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooserbutton));
    gtk_image_set_from_file(GTK_IMAGE(data), filename);
}

// Fonction principale
int main(int argc, char *argv[]) {
    // Initialisation de GTK+
    gtk_init(&argc, &argv);

    // Création de la fenêtre principale
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Affichage d'une image");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_widget_set_size_request(window, 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Création du GtkFileChooserButton pour sélectionner l'image
    GtkWidget *file_chooser_button = gtk_file_chooser_button_new("Sélectionner une image", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_chooser_button), g_get_home_dir());
    gtk_widget_set_hexpand(file_chooser_button, TRUE);

    // Création du GtkImage pour afficher l'image sélectionnée
    GtkWidget *image = gtk_image_new();
    gtk_widget_set_hexpand(image, TRUE);
    gtk_widget_set_vexpand(image, TRUE);

    // Connecter la fonction de rappel pour mettre à jour l'image lors de la sélection du fichier
    g_signal_connect(file_chooser_button, "file-set", G_CALLBACK(on_file_selection), image);

    // Connecter la fonction de rappel pour détecter les clics de souris sur la fenêtre principale
    g_signal_connect(window, "button-press-event", G_CALLBACK(on_mouse_button_press), image);

    // Création d'une boîte verticale pour organiser les widgets
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), file_chooser_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), image, TRUE, TRUE, 0);

    // Ajout de la boîte verticale à la fenêtre principale
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Affichage de la fenêtre
    gtk_widget_show_all(window);

    // Lancement de la boucle principale GTK+
    gtk_main();

    return 0;
}
