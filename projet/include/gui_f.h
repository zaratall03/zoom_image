#ifndef GUI_F_H
#define GUI_F_H
#include <gtk/gtk.h>
#include "interpolation.h"

typedef struct {
    GtkWidget *label_algo1;
    GtkWidget *label_algo2;
    GtkWidget *label_algo3;
} AppWidgets;


void initMainWindow(GtkWidget *window);
void initializeResultTab();
void update_labels(int res1, int res2, int res3, AppWidgets *widgets);


#endif