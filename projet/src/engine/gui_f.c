#include "gui_f.h"
#include "interpolation.h"
#include <gtk/gtk.h>

extern ResultTab resultTab;
extern pthread_mutex_t lock;
extern ZoomType displayedZoomType;



void initMainWindow(GtkWidget *window) {
    if (!GTK_IS_WIDGET(window)) {
        printf("Erreur : le widget n'est pas une fenêtre GTK.\n");
        return;
    }
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_title(GTK_WINDOW(window), "Comparateur de Zoom");
}


void initializeResultTab() {
    pthread_mutex_lock(&lock);
    resultTab.nbAlgo = NB_TYPE;
    printf("%d", resultTab.nbAlgo);
    for (ZoomType zoom = BILINEAR; zoom <= NEAREST_NEIGHBOR; zoom++){
        resultTab.results[zoom].start.tv_sec = 0;
        resultTab.results[zoom].start.tv_nsec = 0;
        resultTab.results[zoom].end.tv_sec = 0;
        resultTab.results[zoom].end.tv_nsec = 0;
        resultTab.results[zoom].zoomType = zoom;
    }
    pthread_mutex_unlock(&lock);
}

void update_labels(int res1, int res2, int res3, AppWidgets *widgets) {
    char buffer[20]; 
    sprintf(buffer, "%d", res1);
    gtk_label_set_text(GTK_LABEL(widgets->label_algo1), buffer);

    sprintf(buffer, "%d", res2);
    gtk_label_set_text(GTK_LABEL(widgets->label_algo2), buffer);

    sprintf(buffer, "%d", res3);
    gtk_label_set_text(GTK_LABEL(widgets->label_algo3), buffer);
}









