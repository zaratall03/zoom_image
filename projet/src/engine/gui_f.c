#include "gui_f.h"
#include "interpolation.h"
#include <gtk/gtk.h>

extern ResultTab resultTab;
extern pthread_mutex_t lock;
extern ZoomType displayedZoomType;

extern GtkBuilder *builder;
extern GtkWidget *labels[NB_TYPE]; // Initialisez tous les éléments à NULL pour éviter les pointeurs non initialisés
extern GtkWidget *histogramDrawingArea;



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




void update_labels(double* res) {
    char buffer[20]; 

    // Récupérer dynamiquement chaque label à partir de son identifiant
    GtkWidget *label_PPVRes = GTK_WIDGET(gtk_builder_get_object(builder, "PPVRes"));
    GtkWidget *label_BilinearRes = GTK_WIDGET(gtk_builder_get_object(builder, "BilinearRes"));
    GtkWidget *label_HermiteRes = GTK_WIDGET(gtk_builder_get_object(builder, "HermiteRes"));

    // Convertir chaque résultat en chaîne de caractères
    sprintf(buffer, "%f", res[NEAREST_NEIGHBOR]);
    gtk_label_set_text(GTK_LABEL(label_PPVRes), buffer);

    sprintf(buffer, "%f", res[BILINEAR]);
    gtk_label_set_text(GTK_LABEL(label_BilinearRes), buffer);

    sprintf(buffer, "%f", res[HERMITE]);
    gtk_label_set_text(GTK_LABEL(label_HermiteRes), buffer);

    // Forcer le redessin des labels
    gtk_widget_queue_draw(label_PPVRes);
    gtk_widget_queue_draw(label_BilinearRes);
    gtk_widget_queue_draw(label_HermiteRes);
}




// HistogramData calculateHistogram(Image image) {
//     HistogramData histogramData;
//     // Initialisation de l'histogramme à zéro
//     memset(histogramData.histogram, 0, sizeof(histogramData.histogram));

//     // Calcul de l'histogramme en parcourant tous les pixels de l'image
//     for (int i = 0; i < image.width * image.height; i++) {
//         unsigned char pixelValue = image.data[i];
//         histogramData.histogram[pixelValue]++;
//     }

//     return histogramData;
// }


// gboolean drawHistogram(GtkWidget *widget, cairo_t *cr, gpointer data) {
//     HistogramData *histogramData = (HistogramData *)data;
//     GtkAllocation allocation;
//     gtk_widget_get_allocation(widget, &allocation);

//     // Dessin des barres de l'histogramme
//     for (int i = 0; i < 256; i++) {
//         double barHeight = histogramData->histogram[i] * allocation.height / (double)(image.width * image.height);
//         cairo_rectangle(cr, i, allocation.height - barHeight, 1, barHeight);
//         cairo_fill(cr);
//     }
//     gtk_widget_queue_draw(histogramDrawingArea)
//     return FALSE;
// }






