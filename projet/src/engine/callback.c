#include "callback.h"
#include "interpolation.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MAX_CHANNELS 4
#define DEFAULT_ZOOM_FACTOR 2

extern Image imageSrc;
extern GtkWidget *image;
extern Image zoomedImage;
extern MouseCallbackData *callbackData;

/**
 * La fonction onFileSelection gère la sélection d'un fichier image, charge l'image sélectionnée et la
 * configure pour qu'elle soit affichée dans un widget d'image GTK.
 * 
 * @param fileChooserButton Le widget GtkFileChooserButton permettant de choisir un fichier via une
 *                          boîte de dialogue.
 * @param data              Un pointeur vers des données supplémentaires pouvant être utilisées par la
 *                          fonction de rappel.
 */
void on_file_selection(GtkFileChooserButton *filechooserbutton, gpointer data) {
    const gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooserbutton));
    char *filename_char = g_strdup(filename);
    imageSrc = loadImage(filename_char);
    gtk_image_set_from_file(GTK_IMAGE(image), filename);
    g_free(filename_char);
}

/**
 * La fonction onMouseButtonPress gère les événements de pression sur le bouton de la souris pour zoomer
 * sur une image affichée dans un widget GtkImage en utilisant l'interpolation bilinéaire.
 * 
 * @param widget Le GtkWidget où l'événement s'est produit.
 * @param event Un GdkEventButton contenant des informations sur l'événement de pression sur le bouton de
 *              la souris.
 * @param userData Un pointeur vers des données utilisateur pouvant être transmises lors de la connexion du
 *                 gestionnaire de signal au widget.
 * 
 * @return La fonction renvoie FALSE.
 */
gboolean on_mouse_button_press(GtkWidget *widget, GdkEventButton *event, gpointer userData) {
    MouseCallbackData *callbackData = (MouseCallbackData *)userData;
    ZoomType zoomType = callbackData->zoomType;

    if (event->type == GDK_BUTTON_PRESS && event->button == 1) {
        gdouble x = event->x;
        gdouble y = event->y;

        GtkAllocation allocation;
        gtk_widget_get_allocation(image, &allocation);

        int image_width = allocation.width;
        int image_height = allocation.height;
        double nearest_x, nearest_y;

        if (x >= 0 && x < image_width && y >= 0 && y < image_height) {
            nearest_x = x;
            nearest_y = y;
        } else {
            nearest_x = CLAMP(x, 0, image_width - 1);
            nearest_y = CLAMP(y, 0, image_height - 1);
        }

        float zoomFactor = DEFAULT_ZOOM_FACTOR; 
        switch (zoomType) {
            case BILINEAR:
                zoomedImage = zoomBilinear(imageSrc, zoomFactor, nearest_x, nearest_y);
                break;
            case HERMITE:
                zoomedImage = zoomHermite(imageSrc, zoomFactor, nearest_x, nearest_y);
                break;
            case NEAREST_NEIGHBOR:
                zoomedImage = zoomNearestNeighbor(imageSrc, zoomFactor, nearest_x, nearest_y);
                break;
            default : 
                printf("Aucun zoom séléctionné !");
                return FALSE;

        }
        
        GdkPixbuf *pixbuf = convertImageToPixbuf(zoomedImage);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
        freeImage(zoomedImage);
        g_object_unref(pixbuf); 
    }
    return FALSE;
}


/**
 * La fonction convertImageToPixbuf convertit une structure de données Image en GdkPixbuf en copiant
 * les données de pixels et en gérant les différences de canaux.
 * 
 * @param image La structure Image contenant des informations sur une image, telles que sa largeur,
 *              sa hauteur, ses données en pixels et le nombre de canaux de couleur.
 * 
 * @return Un pointeur vers une structure GdkPixbuf représentant une image dans la bibliothèque GTK.
 */
GdkPixbuf* convertImageToPixbuf(Image image) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, image.width, image.height);
    if (pixbuf == NULL) {
        g_printerr("Erreur lors de la création du GdkPixbuf.\n");
        return NULL;
    }
    unsigned char *pixels = image.data;
    int channels = image.channels;
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar *buf_pixels = gdk_pixbuf_get_pixels(pixbuf);
    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            int image_index = (y * image.width + x) * channels;
            int pixbuf_index = y * rowstride + x * channels;
            for (int c = 0; c < channels; c++) {
                buf_pixels[pixbuf_index + c] = pixels[image_index + c];
            }
            if (channels < 4) {
                buf_pixels[pixbuf_index + channels] = 255;
            }
        }
    }

    return pixbuf;
}



/**
 * La fonction onComboBoxChanged définit la variable zoomType en fonction de l'élément actif dans
 * une GtkComboBox.
 * 
 * @param combobox  Un pointeur vers le widget GtkComboBox qui a déclenché le signal.
 * @param user_data Un pointeur vers des données définies par l'utilisateur qui peuvent être transmises
 *                  à la fonction de rappel.
 */
void on_combobox_changed(GtkComboBox *combobox, gpointer user_data) {
    callbackData->zoomType = gtk_combo_box_get_active(combobox);
}