#include "gui_f.h"
#include "interpolation.h"
#include <gtk/gtk.h>

extern ResultTab resultTab;
extern pthread_mutex_t lock;
extern ZoomType displayedZoomType;

extern GtkBuilder *builder;
extern GtkWidget *labels[NB_TYPE]; 
extern char* algo_displayed[];

/**
 * La fonction `initializeResultTab` initialise la structure `resultTab` avec des valeurs par défaut
 * pour chaque type de zoom.
 */
void initializeResultTab() {  
    pthread_mutex_lock(&lock);
    resultTab.nbAlgo = NB_TYPE;
    for (ZoomType zoom = NEAREST_NEIGHBOR; zoom <= BICUBIQUE; zoom++){
        resultTab.results[zoom].start.tv_sec = 0;
        resultTab.results[zoom].start.tv_nsec = 0;
        resultTab.results[zoom].end.tv_sec = 0;
        resultTab.results[zoom].end.tv_nsec = 0;
        resultTab.results[zoom].zoomType = zoom;
    }
    pthread_mutex_unlock(&lock);
}


/**
 * Initialise une fenêtre GTK avec des propriétés spécifiques.
 * 
 * @param window Pointeur vers le widget GtkWidget représentant la fenêtre principale GTK.
 * 
 * @return Si le paramètre window n'est pas un widget GTK, affiche un message d'erreur et retourne sans effectuer d'autres actions.
 */
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


/**
 * Met à jour les étiquettes de texte dans une application GTK avec les valeurs d'un tableau et déclenche un redessin des étiquettes.
 * 
 * @param res   Pointeur vers un tableau utilisé pour mettre à jour le texte des étiquettes GTK.
 */
void update_labels(double* res) {
    char buffer[20]; 
    GtkWidget *label_PPVRes = GTK_WIDGET(gtk_builder_get_object(builder, "PPVRes"));
    GtkWidget *label_BilinearRes = GTK_WIDGET(gtk_builder_get_object(builder, "BilinearRes"));
    // GtkWidget *label_HermiteRes = GTK_WIDGET(gtk_builder_get_object(builder, "HermiteRes"));
    GtkWidget *label_BiCubiqueRes = GTK_WIDGET(gtk_builder_get_object(builder, "BiCubiqueRes"));    
    

    sprintf(buffer, "%f", res[NEAREST_NEIGHBOR]);
    gtk_label_set_text(GTK_LABEL(label_PPVRes), buffer);

    sprintf(buffer, "%f", res[BILINEAR]);
    gtk_label_set_text(GTK_LABEL(label_BilinearRes), buffer);

    // sprintf(buffer, "%f", res[HERMITE]);
    // gtk_label_set_text(GTK_LABEL(label_HermiteRes), buffer);

    sprintf(buffer, "%f", res[BICUBIQUE]);
    gtk_label_set_text(GTK_LABEL(label_BiCubiqueRes), buffer);

    
    gtk_widget_queue_draw(label_PPVRes);
    gtk_widget_queue_draw(label_BilinearRes);
    // gtk_widget_queue_draw(label_HermiteRes);
    gtk_widget_queue_draw(label_BiCubiqueRes);
    update_displayed_type();
}


/**
 * La fonction `update_displayed_type` met à jour le type d'algorithme affiché dans une étiquette GTK
 * en fonction du type de zoom actuel.
 */
void update_displayed_type(){
    char algoBuffer[50]; 
    GtkWidget *displayed_res = GTK_WIDGET(gtk_builder_get_object(builder, "displayedAlgo"));    
    ZoomType type = getdisplayedZoomType();
    strcpy(algoBuffer, algo_displayed[type]);
    gtk_label_set_text(GTK_LABEL(displayed_res), algoBuffer);
    gtk_widget_queue_draw(displayed_res);
}



