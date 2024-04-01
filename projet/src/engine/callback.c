#include  <gtk/gtk.h>
#include <pthread.h>

#include "callback.h"
#include <pthread.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MAX_CHANNELS 4


#define DEFAULT_ZOOM_FACTOR 1.05

extern gboolean zoomInClicked;
extern gboolean zoomOutClicked;
extern ResultTab resultTab;
extern short uploaded; 
extern ZoomType displayedZoomType;
extern cairo_t *cr;
extern GtkWidget *drawing_area;


extern GtkWidget * labels[NB_TYPE];


Zoom zoomInList[NB_TYPE] = {
    zoomNearestNeighbor,
    zoomBilinear,
    zoomBicubic,

};

Zoom zoomOutList[NB_TYPE] = {
    zoomOutNearestNeighbor,
    zoomOutBilinear,
    zoomOutBicubic,
};

/**
 * Ouvre une boîte de dialogue de sélection de fichier pour choisir une image, la charge, la traite et l'affiche dans un widget d'image GTK.
 * 
 * @param menu_item     Pointeur vers le widget GtkMenuItem ayant déclenché la fonction open_file.
 * @param user_data     Pointeur vers les données utilisateur, utilisé ici pour passer un pointeur vers un GtkWidget représentant l'image sélectionnée.
 */
void open_file(GtkMenuItem *menu_item, gpointer user_data) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Open File", NULL, action, "_Cancel",
                                         GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pixbuf_formats(filter); 
    gtk_file_filter_set_name(filter, "Image Files");
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);
    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        uploaded = 1;
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        Image img = loadImage(filename);
        printf("\n\nNombre de canneaux: %d \n\n", img.channels);
        for(int type = 0 ; type < NB_TYPE; type++){
            setImageFromResult(type, img);
        }
        GtkWidget *image = GTK_WIDGET(user_data);
        gtk_image_set_from_file(GTK_IMAGE(image), filename); 
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}


/**
 * Ouvre une boîte de dialogue de sélection de fichier pour enregistrer une image au format JPG.
 * 
 * @param menu_item     Pointeur vers le GtkMenuItem ayant déclenché l'action de sauvegarde.
 * @param user_data     Pointeur vers les données utilisateur.
 */
void save_file(GtkMenuItem *menu_item, gpointer user_data) {
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Enregistrer l'image",
                                         GTK_WINDOW(user_data),
                                         action,
                                         "_Annuler",
                                         GTK_RESPONSE_CANCEL,
                                         "_Enregistrer",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);

    chooser = GTK_FILE_CHOOSER(dialog);

    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(chooser);

        ZoomType type = getdisplayedZoomType();
        Image img =  getImageFromResult(type);
        writeImageJpg(filename, img);

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

/**
 * Définit l'état actif d'un bouton GTK en fonction d'une valeur booléenne.
 * 
 * @param button        Pointeur vers un widget GtkButton.
 * @param is_clicked    Valeur booléenne indiquant si le bouton doit être dans un état cliqué ou non.
 *                      TRUE pour un état cliqué, FALSE pour un état non cliqué.
 */
void update_button_state(GtkButton *button, gboolean is_clicked) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), is_clicked);
}


void on_zoom_in_clicked(GtkButton *button, gpointer user_data) {
    if (!zoomInClicked) {
        zoomInClicked = TRUE;
        zoomOutClicked = FALSE;
        update_button_state(GTK_BUTTON(user_data), zoomOutClicked);
        update_button_state(GTK_BUTTON(button), zoomInClicked);
    }
}

void on_zoom_out_clicked(GtkButton *button, gpointer user_data) {
    if (!zoomOutClicked) {
        zoomOutClicked = TRUE;
        zoomInClicked = FALSE;
        update_button_state(GTK_BUTTON(user_data), zoomInClicked);
        update_button_state(GTK_BUTTON(button), zoomOutClicked);
    }
}

/**
 * Convertit une image en objet GdkPixbuf.
 * 
 * @param image Les données de l'image à convertir en GdkPixbuf.
 * 
 * @return Un pointeur GdkPixbuf* représentant l'image convertie.
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
 * Gère l'événement de changement d'un GtkComboBox, mettant à jour l'image affichée en fonction de la sélection.
 * 
 * @param combo_box Pointeur vers le widget GtkComboBox qui a déclenché le signal.
 * @param user_data Pointeur vers les données utilisateur.
 */
void on_combo_box_changed(GtkComboBox *combo_box, gpointer user_data) {
    GtkTreeModel *model = gtk_combo_box_get_model(combo_box);
        gint active_index = gtk_combo_box_get_active(combo_box);

    if (active_index >= 0) {
        GtkTreeIter iter;
            if (gtk_tree_model_iter_nth_child(model, &iter, NULL, active_index)) {
            gint selected_value = 0;
            gtk_tree_model_get(model, &iter, 1, &selected_value, -1);
            setdisplayedZoomType(selected_value);
            if (uploaded) {
                Image current = getImageFromResult(selected_value);
                GdkPixbuf *zoomedPixbuf = convertImageToPixbuf(current);
                if (zoomedPixbuf != NULL) {
                    GtkImage *gtkImage = GTK_IMAGE(user_data);                    
                    gtk_image_set_from_pixbuf(gtkImage, zoomedPixbuf);                    
                    g_object_unref(zoomedPixbuf);
                } else {
                    printf("Erreur : Le pixbuf zoomé est NULL.\n");
                }
            }
            update_displayed_type();
        } else {
            g_print("Erreur lors de l'obtention de l'itérateur pour l'élément sélectionné\n");
        }
    } else {
        g_print("Aucun élément sélectionné dans la combobox\n");
    }
}


/**
 * Convertit un GdkPixbuf en structure Image en extrayant les données de pixels.
 * 
 * @param pixbuf    Pointeur vers un GdkPixbuf à convertir en Image.
 * 
 * @return          Structure Image contenant les données extraites du GdkPixbuf.
 */
Image convertPixbufToImage(GdkPixbuf *pixbuf) {
    Image image;
    image.width = gdk_pixbuf_get_width(pixbuf);
    image.height = gdk_pixbuf_get_height(pixbuf);
    image.channels = gdk_pixbuf_get_n_channels(pixbuf);
    image.data = (unsigned char *)g_malloc(image.width * image.height * image.channels * sizeof(unsigned char));
    if (image.data == NULL) {
        g_printerr("Erreur lors de l'allocation de la mémoire pour l'image.\n");
        return image;
    }
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar *buf_pixels = gdk_pixbuf_get_pixels(pixbuf);
    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            int image_index = (y * image.width + x) * image.channels;
            int pixbuf_index = y * rowstride + x * image.channels;
            for (int c = 0; c < image.channels; c++) {
                image.data[image_index + c] = buf_pixels[pixbuf_index + c];
            }
        }
    }
    return image;
}

/**
 * Gère les événements de relâchement du bouton de la souris dans un widget GTK, effectue des opérations de zoom sur l'image et met à jour l'image affichée en conséquence.
 * 
 * @param widget    Pointeur vers le GtkWidget (l'image).
 * @param event     Pointeur vers une structure GdkEventButton contenant des informations sur l'événement de souris.
 * @param image     Pointeur vers le GtkWidget représentant l'image affichée.
 * 
 * @return          TRUE
 */
gboolean on_mouse_button_release(GtkWidget *widget, GdkEventButton *event, GtkWidget *image) {
    if (image == NULL || (!zoomInClicked && !zoomOutClicked)) {
        return TRUE;
    }
    Image imgRes; 

    if (event != NULL && event->type == GDK_BUTTON_RELEASE && event->button == 1) {
        float zoomFactor = 1.2;
        
        pthread_t threads[NB_TYPE];
        for (ZoomType zoom = NEAREST_NEIGHBOR; zoom <= BICUBIQUE; zoom++){
            ThreadArgs *args = malloc(sizeof(ThreadArgs));
            if (args == NULL) {
                fprintf(stderr, "Erreur lors de l'allocation de mémoire pour les arguments du thread.\n");
                exit(EXIT_FAILURE);
            }
            args->type = zoom;
            args->zoomFactor = zoomFactor;
            args->zoomFunc =  zoomInClicked ? zoomInList[zoom] : zoomOutList[zoom]; 
            imgRes = getImageFromResult(zoom);
            args->resultImage = &imgRes;
            if (pthread_create(&threads[zoom], NULL, timed_zoom_thread, args) != 0) {
                fprintf(stderr, "Erreur lors de la création du thread pour le zoom %d\n", zoom);
            }
        }

        for (ZoomType zoom = NEAREST_NEIGHBOR; zoom <= BICUBIQUE; zoom++){
            if (pthread_join(threads[zoom], NULL) != 0) {
                fprintf(stderr, "Erreur lors de l'attente du thread pour le zoom %d\n", zoom);
            }
        }
        ZoomType displayedZoom = getdisplayedZoomType();
        imgRes = getImageFromResult(displayedZoom);
        GdkPixbuf *zoomedPixbuf = convertImageToPixbuf(imgRes);
        if (zoomedPixbuf != NULL) {
            gtk_image_set_from_pixbuf(GTK_IMAGE(image), zoomedPixbuf);
            g_object_unref(zoomedPixbuf);
        } else {
            printf("Erreur : Le pixbuf zoomé est NULL.\n");
        }
        double res[NB_TYPE];
        struct timespec start, end;

        for(int t = 0; t< NB_TYPE; t++){
            start = getStartFromResult(t); 
            end = getEndFromResult(t);
            res[t] = calculateElapsedTime(start, end);
        }
        update_labels(res);
        gtk_widget_queue_draw(drawing_area);
    }
    return TRUE;
}



/**
 * Effectue un zoom sur une image, mesure le temps nécessaire à l'opération et met à jour l'image résultante.
 * 
 * @param args Pointeur vers une structure ThreadArgs contenant les arguments pour le zoom.
 */
void *timed_zoom_thread(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    ZoomType type = threadArgs->type;
    float zoomFactor = threadArgs->zoomFactor;
    Zoom zoomFunc = threadArgs->zoomFunc;

    Image img = getImageFromResult(type);
    Image *resultImage = &img;
    struct timespec start, end; 

    clock_gettime(CLOCK_MONOTONIC, &start);
    setStartFromResult(type, start);    
    *resultImage = zoomFunc(*resultImage, zoomFactor);
    setImageFromResult(type, *resultImage);
    clock_gettime(CLOCK_MONOTONIC, &end);
    setEndFromResult(type, end);
    free(args);
    pthread_exit(NULL);
}


/**
 * Effectue une opération de zoom sur une image avec un type de zoom, un facteur de zoom et une fonction de zoom spécifiés, et enregistre les heures de début et de fin de l'opération.
 * 
 * @param type       Type d'opération de zoom à effectuer.
 * @param zoomFactor Facteur de zoom spécifiant dans quelle mesure l'image doit être agrandie ou réduite.
 * @param zoomFunc   Pointeur de fonction représentant la fonction chargée d'effectuer l'opération de zoom sur l'image.
 */ 
void timed_zoom(ZoomType type, float zoomFactor, Zoom zoomFunc) {
    Image img = getImageFromResult(type);
    struct timespec start, end; 
    clock_gettime(CLOCK_MONOTONIC, &start);
    setStartFromResult(type, start);
    img = zoomFunc(img, zoomFactor);
    setImageFromResult(type, img);
    clock_gettime(CLOCK_MONOTONIC, &end);
    setEndFromResult(type, end);
}


/**
 * Dessine un histogramme des intensités de pixels à partir d'une image sur un widget GTK utilisant un contexte Cairo.
 * 
 * @param widget Le widget GTK sur lequel l'histogramme sera dessiné.
 * @param cr     Le contexte graphique Cairo utilisé pour dessiner l'histogramme.
 * @param data   Le widget GtkImage contenant l'image pour laquelle l'histogramme est dessiné.
 */ 
gboolean draw_histogram(GtkWidget *widget, cairo_t *cr, gpointer data) {
    if(uploaded){

        GtkImage *image = GTK_IMAGE(data);
        GdkPixbuf *pixbuf = gtk_image_get_pixbuf(image);
        int widget_width = gtk_widget_get_allocated_width(widget);
        int widget_height = widget_width * gdk_pixbuf_get_height(pixbuf) / gdk_pixbuf_get_width(pixbuf);

        int histogram[256] = {0};
        int channels = gdk_pixbuf_get_n_channels(pixbuf);
        int width = gdk_pixbuf_get_width(pixbuf);
        int height = gdk_pixbuf_get_height(pixbuf);
        guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
        guchar *p;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                p = pixels + y * gdk_pixbuf_get_rowstride(pixbuf) + x * channels;
                int intensity = (p[0] + p[1] + p[2]) / 3;
                histogram[intensity]++;
            }
        }

        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_paint(cr);
        cairo_set_source_rgb(cr, 0, 0, 0); 
        cairo_set_line_width(cr, 1);

        double max_value = 0;
        for (int i = 0; i < 256; i++) {
            if (histogram[i] > max_value) {
                max_value = histogram[i];
            }
        }

        double scale_factor = (double)widget_height / max_value;
        double bar_width = (double)widget_width / 256;

        for (int i = 0; i < 256; i++) {
            double x = i * bar_width;
            double y = widget_height - histogram[i] * scale_factor;
            cairo_rectangle(cr, x, y, bar_width, histogram[i] * scale_factor);
            cairo_fill(cr);
            cairo_stroke(cr);
        }
        return FALSE;
    }
}

void on_click_quit(GtkWidget *button, gpointer window) {
    gtk_widget_destroy(GTK_WIDGET(window));
}

