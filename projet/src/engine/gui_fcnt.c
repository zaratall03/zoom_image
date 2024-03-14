#include "gui_fcnt.h"
#include "interpolation.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MAX_CHANNELS 4

extern Image imageSrc; // Déclarez les variables globales comme extern
extern GtkWidget *image; // Variable globale pour le widget GtkImage
extern Image zoomedImage; // Déclarez les variables globales comme extern


/**
 * La fonction `on_file_selection` gère la sélection d'un fichier image, charge l'image sélectionnée et
 * la configure pour qu'elle soit affichée dans un widget d'image GTK.
    * 
 * @param filechooserbutton Le paramètre `filechooserbutton` est un pointeur vers un widget
 * `GtkFileChooserButton`, qui permet de choisir un fichier via une boîte de dialogue.
 * @param data Le paramètre `data` est un pointeur vers des données supplémentaires pouvant être utilisées
 * par la fonction de rappel.
 */
// Fonction de rappel pour gérer la sélection d'un fichier image
void on_file_selection(GtkFileChooserButton *filechooserbutton, gpointer data) {
    const gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooserbutton));
    char *filename_char = g_strdup(filename);
    // Charger l'image sélectionnée et l'affecter à imageSrc
    imageSrc = loadImage(filename_char);
    gtk_image_set_from_file(GTK_IMAGE(image), filename);
    g_free(filename_char);
}

// Fonction de rappel pour gérer les clics de souris et effectuer le zoom
/**
 * La fonction gère les événements de pression sur le bouton de la souris pour zoomer sur une image
 * affichée dans un widget GtkImage en utilisant l'interpolation bilinéaire.
 * 
 * @param widget Le paramètre `widget` est un pointeur vers le GtkWidget où l'événement s'est produit.
 * @param event Le paramètre `event` est un GdkEventButton contenant des informations sur l'événement de
 * pression sur le bouton de la souris.
 * @param data Le paramètre `data` est un pointeur vers des données utilisateur pouvant être transmises
 * lors de la connexion du gestionnaire de signal au widget.
 * 
 * @return La fonction renvoie `FALSE`.
 */
gboolean on_mouse_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    if (event->type == GDK_BUTTON_PRESS && event->button == 1) {
        gdouble x = event->x;
        gdouble y = event->y;

        GtkAllocation allocation;
        gtk_widget_get_allocation(image, &allocation);

        int image_width = allocation.width;
        int image_height = allocation.height;
        double nearest_x, nearest_y;

        // Vérifier si le clic de souris est à l'intérieur de l'image
        if (x >= 0 && x < image_width && y >= 0 && y < image_height) {
            nearest_x = x;
            nearest_y = y;
        } else {
            // Si le clic est en dehors de l'image, ajuster les coordonnées
            nearest_x = CLAMP(x, 0, image_width - 1);
            nearest_y = CLAMP(y, 0, image_height - 1);
        }

        float zoomFactor = 2.0; // Changer le facteur de zoom selon vos besoins
        // Appliquer le zoom bilinéaire à l'image source et obtenir l'image zoomée
        zoomedImage = bilinear(&imageSrc, zoomFactor, nearest_x, nearest_y, image_width, image_height); 
        
        // Convertir l'image zoomée en GdkPixbuf
        GdkPixbuf *pixbuf = convertImageToPixbuf(zoomedImage);
        // Mettre à jour le GtkImage avec l'image zoomée
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
        // Libérer la mémoire allouée pour l'image zoomée
        freeImage(zoomedImage);
        g_object_unref(pixbuf); // Libérer le GdkPixbuf après utilisation
    }
    return FALSE;
}

/**
 * La fonction `convertImageToPixbuf` convertit une structure de données Image en GdkPixbuf en copiant
 * les données de pixels et en gérant les différences de canaux.
 * 
 * @param image La structure Image contenant des informations sur une image, telles que sa largeur,
 * sa hauteur, ses données en pixels et le nombre de canaux de couleur.
 * 
 * @return Un pointeur vers une structure GdkPixbuf représentant une image dans la bibliothèque GTK.
 */
// Fonction pour convertir une Image en GdkPixbuf
GdkPixbuf* convertImageToPixbuf(Image image) {
    // Créer un GdkPixbuf avec les dimensions de l'image et le format RGB ou RGBA en fonction du nombre de canaux
    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, image.width, image.height);
    if (pixbuf == NULL) {
        g_printerr("Erreur lors de la création du GdkPixbuf.\n");
        return NULL;
    }

    // Obtenir les données des pixels de l'image
    unsigned char *pixels = image.data;

    // Obtenir le nombre de canaux de l'image
    int channels = image.channels;

    // Obtenir la largeur de ligne (rowstride) du GdkPixbuf
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);

    // Obtenir les canaux de couleur du GdkPixbuf
    guchar *buf_pixels = gdk_pixbuf_get_pixels(pixbuf);

    // Copier les données des pixels de l'image dans le GdkPixbuf
    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            // Calculer l'index du pixel dans l'image
            int image_index = (y * image.width + x) * channels;

            // Calculer l'index du pixel dans le GdkPixbuf
            int pixbuf_index = y * rowstride + x * channels;

            // Copier les valeurs des canaux de couleur de l'image vers le GdkPixbuf
            for (int c = 0; c < channels; c++) {
                buf_pixels[pixbuf_index + c] = pixels[image_index + c];
            }

            // Si l'image a moins de 4 canaux, définir la valeur du canal alpha du GdkPixbuf sur 255 (pleinement opaque)
            if (channels < 4) {
                buf_pixels[pixbuf_index + channels] = 255;
            }
        }
    }

    return pixbuf;
}
