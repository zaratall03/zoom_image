#include  <gtk/gtk.h>
#include <pthread.h>

#include "callback.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MAX_CHANNELS 4
#define DEFAULT_ZOOM_FACTOR 2


#define MAX_ZOOMED_WIDTH 800
#define MAX_ZOOMED_HEIGHT 800

extern gboolean zoomInClicked;
extern gboolean zoomOutClicked;
extern ResultTab resultTab;
extern pthread_mutex_t lock;


Zoom zoomInList[NB_TYPE] = {
    zoomBilinear,
    zoomNearestNeighbor,
    zoomHermite
};

Zoom zoomOutList[NB_TYPE] = {
    zoomOutBilinear,
    zoomOutNearestNeighbor,
    zoomOutHermite
};

void open_file(GtkMenuItem *menu_item, gpointer user_data) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Open File", NULL, action, "_Cancel",
                                         GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        Image img = loadImage(filename);
        pthread_mutex_lock(&lock);
        for(int type = 0 ; type < resultTab.nbAlgo; type++){
            resultTab.results[type].resultImage = img;
        }
        pthread_mutex_unlock(&lock);
        GtkWidget *image = GTK_WIDGET(user_data);
        gtk_image_set_from_file(GTK_IMAGE(image), filename); 
        g_free(filename);
    }
    
afficheResultTab(resultTab); 
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




void timed_zoom(ZoomType type, float zoomFactor, Zoom zoom){
    pthread_mutex_lock(&lock);
    clock_gettime(CLOCK_MONOTONIC, &resultTab.results[type].start);
    pthread_mutex_unlock(&lock);
    Image img = resultTab.results[type].resultImage; 
    resultTab.results[type].resultImage = zoom(img, zoomFactor); 
    pthread_mutex_lock(&lock);
    clock_gettime(CLOCK_MONOTONIC, &resultTab.results[type].end);
    pthread_mutex_unlock(&lock);
}




gboolean on_mouse_button_release(GtkWidget *widget, GdkEventButton *event, GtkWidget *image) {
    if (image == NULL || (!zoomInClicked && !zoomOutClicked)) {
        return TRUE;
    }

    if (event != NULL && event->type == GDK_BUTTON_RELEASE && event->button == 1) {
        GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));
        Image img = convertPixbufToImage(pixbuf);
        float zoomFactor = 1.2;
        ZoomType type = BILINEAR;
        
        pthread_t threads[NB_TYPE];
        for (int zoom = 0; zoom < resultTab.nbAlgo; zoom++) {
            ThreadArgs *args = malloc(sizeof(ThreadArgs));
            if (args == NULL) {
                fprintf(stderr, "Erreur lors de l'allocation de mémoire pour les arguments du thread.\n");
                exit(EXIT_FAILURE);
            }
            args->type = (ZoomType)zoom;
            args->zoomFactor = zoomFactor;
            args->zoomFunc = zoomInClicked ? zoomInList[zoom] : zoomOutList[zoom];
            args->resultImage = &(resultTab.results[zoom].resultImage);
            if (pthread_create(&threads[zoom], NULL, timed_zoom_thread, args) != 0) {
                fprintf(stderr, "Erreur lors de la création du thread pour le zoom %d\n", zoom);
            }
        }

        for (int zoom = 0; zoom < resultTab.nbAlgo; zoom++) {
            if (pthread_join(threads[zoom], NULL) != 0) {
                fprintf(stderr, "Erreur lors de l'attente du thread pour le zoom %d\n", zoom);
            }
        }
        
        GdkPixbuf *zoomedPixbuf = convertImageToPixbuf(resultTab.results[type].resultImage);
        if (zoomedPixbuf != NULL) {
            gtk_image_set_from_pixbuf(GTK_IMAGE(image), zoomedPixbuf);
            g_object_unref(zoomedPixbuf);
        } else {
            printf("Erreur : Le pixbuf zoomé est NULL.\n");
        }
        afficheResultTab(resultTab);

        if (img.data != NULL) {
            g_free(img.data);
        }
    }

    return TRUE;
}



void *timed_zoom_thread(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    ZoomType type = threadArgs->type;
    float zoomFactor = threadArgs->zoomFactor;
    Zoom zoomFunc = threadArgs->zoomFunc;
    Image *resultImage = threadArgs->resultImage;

    pthread_mutex_lock(&lock);
    clock_gettime(CLOCK_MONOTONIC, &resultTab.results[type].start);
    pthread_mutex_unlock(&lock);
    Image img = *resultImage;
    *resultImage = zoomFunc(img, zoomFactor);
    pthread_mutex_lock(&lock);
    clock_gettime(CLOCK_MONOTONIC, &resultTab.results[type].end);
    pthread_mutex_unlock(&lock);

    free(args);
    pthread_exit(NULL);
}
