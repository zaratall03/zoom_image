#ifndef GUI_F_H
#define GUI_F_H
#include <gtk/gtk.h>
#include "interpolation.h"



void initMainWindow(GtkWidget *window);
void initializeResultTab();
void update_labels(double* res);
void update_displayed_type();

#endif