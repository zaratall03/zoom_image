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
        for(int type = 0 ; type < NB_TYPE; type++){
            setImageFromResult(type, img);
        }
        GtkWidget *image = GTK_WIDGET(user_data);
        gtk_image_set_from_file(GTK_IMAGE(image), filename); 
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}


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


// gboolean on_mouse_button_release(GtkWidget *widget, GdkEventButton *event, GtkWidget *image) {
//     if (image == NULL || (!zoomInClicked && !zoomOutClicked)) {
//         return TRUE;
//     }

//     if (event != NULL && event->type == GDK_BUTTON_RELEASE && event->button == 1) {
//         float zoomFactor = DEFAULT_ZOOM_FACTOR;
//         pthread_t threads[NB_TYPE];
//         ThreadArgs *threadArgsList[NB_TYPE];

//         // Création des threads pour chaque type de zoom
//         for (ZoomType zoom = NEAREST_NEIGHBOR; zoom <= BICUBIQUE; zoom++) {
//             threadArgsList[zoom] = (ThreadArgs *)malloc(sizeof(ThreadArgs));
//             if (threadArgsList[zoom] == NULL) {
//                 printf("Erreur : échec d'allocation de mémoire pour threadArgsList.\n");
//                 // Libération de la mémoire allouée pour les threads précédemment créés
//                 for (ZoomType z = NEAREST_NEIGHBOR; z < zoom; z++) {
//                     free(threadArgsList[z]);
//                 }
//                 return TRUE;
//             }
//             threadArgsList[zoom]->type = zoom;
//             threadArgsList[zoom]->zoomFactor = zoomFactor;
//             threadArgsList[zoom]->zoomFunc = zoomInClicked ? zoomInList[zoom] : zoomOutList[zoom];
//             Image imgToUpdate = getImageFromResult(zoom);
//             threadArgsList[zoom]->resultImage = &imgToUpdate;
//             int ret = pthread_create(&threads[zoom], NULL, timed_zoom_thread, (void *)threadArgsList[zoom]);
//             if (ret != 0) {
//                 printf("Erreur : échec de création du thread.\n");
//                 // Libération de la mémoire allouée pour les threads précédemment créés
//                 for (ZoomType z = NEAREST_NEIGHBOR; z < zoom; z++) {
//                     free(threadArgsList[z]);
//                 }
//                 free(threadArgsList[zoom]);
//                 return TRUE;
//             }
//         }

//         // Attente de la fin de tous les threads
//         for (ZoomType zoom = NEAREST_NEIGHBOR; zoom <= BICUBIQUE; zoom++) {
//             int ret = pthread_join(threads[zoom], NULL);
//             if (ret != 0) {
//                 printf("Erreur : échec de l'attente du thread.\n");
//                 // Possibilité de libérer la mémoire allouée pour les threads précédemment créés ici
//                 return TRUE;
//             }
//             free(threadArgsList[zoom]);
//         }

//         // Mise à jour de l'image affichée
//         ZoomType displayedZ = getdisplayedZoomType();
//         Image imgRes = getImageFromResult(displayedZ);
//         GdkPixbuf *zoomedPixbuf = convertImageToPixbuf(imgRes);
//         if (zoomedPixbuf != NULL) {
//             gtk_image_set_from_pixbuf(GTK_IMAGE(image), zoomedPixbuf);
//             g_object_unref(zoomedPixbuf);
//         } else {
//             printf("Erreur : Le pixbuf zoomé est NULL.\n");
//         }

//         double res[NB_TYPE];
//         for (ZoomType zoom = NEAREST_NEIGHBOR; zoom <= BICUBIQUE; zoom++) {
//             struct timespec start = getStartFromResult(zoom);
//             struct timespec end = getEndFromResult(zoom);
//             res[zoom] = calculateElapsedTime(start, end);
//             printf("%f", res[zoom]);
//             printf("On Modifie %d", zoom);
//         }

//         // Mise à jour des étiquettes
//         update_labels(res);
//     }

//     return TRUE;
// }


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
            args->zoomFunc =  zoomInClicked ? zoomInList[zoom] : zoomOutList[zoom]; // Utiliser la liste appropriée
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
        printf("\nDISPLAYED  ZOOM : %d", displayedZoomType);
        imgRes = getImageFromResult(displayedZoomType);
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
    printf("\nJe finid %d", type);
    *resultImage = zoomFunc(*resultImage, zoomFactor);
    setImageFromResult(type, *resultImage);
    clock_gettime(CLOCK_MONOTONIC, &end);
    setEndFromResult(type, end);
    free(args);
    pthread_exit(NULL);
}


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


#define HISTOGRAM_WIDTH 100
#define HISTOGRAM_HEIGHT 100

// gboolean draw_histogram(GtkWidget *widget, cairo_t *cr, gpointer data) {
//     GtkImage *image = GTK_IMAGE(data);

//     // Obtenir le GdkPixbuf à partir de l'image
//     GdkPixbuf *pixbuf = gtk_image_get_pixbuf(image);

//     // Calculer l'histogramme
//     int histogram[256] = {0};
//     int channels = gdk_pixbuf_get_n_channels(pixbuf);
//     int width = gdk_pixbuf_get_width(pixbuf);
//     int height = gdk_pixbuf_get_height(pixbuf);
//     guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
//     guchar *p;
//     for (int y = 0; y < height; y++) {
//         for (int x = 0; x < width; x++) {
//             p = pixels + y * gdk_pixbuf_get_rowstride(pixbuf) + x * channels;
//             int intensity = (p[0] + p[1] + p[2]) / 3; // Moyenne des canaux RGB
//             histogram[intensity]++;
//         }
//     }

//     // Dessiner l'histogramme
//     cairo_set_source_rgb(cr, 1, 1, 1); // Fond blanc
//     cairo_paint(cr);
//     cairo_set_source_rgb(cr, 0, 0, 0); // Trait noir
//     cairo_set_line_width(cr, 1);

//     double max_value = 0;
//     for (int i = 0; i < 256; i++) {
//         if (histogram[i] > max_value) {
//             max_value = histogram[i];
//         }
//     }

//     double scale_factor = (double)HISTOGRAM_HEIGHT / max_value;
//     double bar_width = (double)HISTOGRAM_WIDTH/ 256;

//     for (int i = 0; i < 256; i++) {
//         double x = i * bar_width;
//         double y = HISTOGRAM_HEIGHT - histogram[i] * scale_factor;
//         cairo_rectangle(cr, x, y, bar_width, histogram[i] * scale_factor);
//         cairo_fill(cr);
//         cairo_stroke(cr);
//     }

//     return FALSE; // Indique que le dessin est terminé
// }


gboolean draw_histogram(GtkWidget *widget, cairo_t *cr, gpointer data) {
    if(uploaded){

        GtkImage *image = GTK_IMAGE(data);

        // Obtenir le GdkPixbuf à partir de l'image
        GdkPixbuf *pixbuf = gtk_image_get_pixbuf(image);

        // Obtenir la largeur du widget
        int widget_width = gtk_widget_get_allocated_width(widget);

        // Calculer la hauteur du widget en fonction de sa largeur
        int widget_height = widget_width * gdk_pixbuf_get_height(pixbuf) / gdk_pixbuf_get_width(pixbuf);

        // Calculer l'histogramme
        int histogram[256] = {0};
        int channels = gdk_pixbuf_get_n_channels(pixbuf);
        int width = gdk_pixbuf_get_width(pixbuf);
        int height = gdk_pixbuf_get_height(pixbuf);
        guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
        guchar *p;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                p = pixels + y * gdk_pixbuf_get_rowstride(pixbuf) + x * channels;
                int intensity = (p[0] + p[1] + p[2]) / 3; // Moyenne des canaux RGB
                histogram[intensity]++;
            }
        }

        // Dessiner l'histogramme
        cairo_set_source_rgb(cr, 1, 1, 1); // Fond blanc
        cairo_paint(cr);
        cairo_set_source_rgb(cr, 0, 0, 0); // Trait noir
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
        printf("Histogramme à jour");
        return FALSE; // Indique que le dessin est terminé
    }
}

