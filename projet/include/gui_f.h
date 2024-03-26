    #ifndef GUI_F_H
#define GUI_F_H
#include <gtk/gtk.h>
#include "interpolation.h"





typedef struct {
    int histogram[256];
} HistogramData;


void initMainWindow(GtkWidget *window);
void initializeResultTab();
void update_labels(double* res);
HistogramData calculateHistogram(Image image); 
gboolean drawHistogram(GtkWidget *widget, cairo_t *cr, gpointer data); 

#endif