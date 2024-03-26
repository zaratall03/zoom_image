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
extern AppWidgets resLabel;
extern ZoomType displayedZoomType;

Zoom zoomInList[NB_TYPE] = {
    zoomBilinear,
    zoomHermite,
    zoomNearestNeighbor,
};

Zoom zoomOutList[NB_TYPE] = {
    zoomOutBilinear,
    zoomOutHermite,
    zoomOutNearestNeighbor,
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
        for(int type = 0 ; type < NB_TYPE; type++){
            setImageFromResult(type, img);
        }
        GtkWidget *image = GTK_WIDGET(user_data);
        gtk_image_set_from_file(GTK_IMAGE(image), filename); 
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


gboolean on_mouse_button_release(GtkWidget *widget, GdkEventButton *event, GtkWidget *image) {
    if (image == NULL || (!zoomInClicked && !zoomOutClicked)) {
        return TRUE;
    }
    Image imgRes; 

    if (event != NULL && event->type == GDK_BUTTON_RELEASE && event->button == 1) {
        float zoomFactor = 1.2;
        
        pthread_t threads[NB_TYPE];
        for (ZoomType zoom = BILINEAR; zoom <= NEAREST_NEIGHBOR; zoom++){
            ThreadArgs *args = malloc(sizeof(ThreadArgs));
            if (args == NULL) {
                fprintf(stderr, "Erreur lors de l'allocation de mémoire pour les arguments du thread.\n");
                exit(EXIT_FAILURE);
            }
            args->type = zoom;
            args->zoomFactor = zoomFactor;
            args->zoomFunc = zoomInClicked ? zoomInList[zoom] : zoomOutList[zoom];
            imgRes = getImageFromResult(zoom);
            args->resultImage = &imgRes;
            if (pthread_create(&threads[zoom], NULL, timed_zoom_thread, args) != 0) {
                fprintf(stderr, "Erreur lors de la création du thread pour le zoom %d\n", zoom);
            }
        }

        for (ZoomType zoom = BILINEAR; zoom <= NEAREST_NEIGHBOR; zoom++){
            if (pthread_join(threads[zoom], NULL) != 0) {
                fprintf(stderr, "Erreur lors de l'attente du thread pour le zoom %d\n", zoom);
            }
        }
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
            printf("res[%d]=%f", t, res[t]); 
        }
        afficheResultTab(resultTab);
    }
    return TRUE;
}



void *timed_zoom_thread(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    ZoomType type = threadArgs->type;
    float zoomFactor = threadArgs->zoomFactor;
    Zoom zoomFunc = threadArgs->zoomFunc;
    Image *resultImage = threadArgs->resultImage;

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

