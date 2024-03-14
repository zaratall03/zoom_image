#ifndef GUI_FNCT_H
#define GUI_FNCT_H

#include <gtk/gtk.h>
#include "image.h"

void on_file_selection(GtkFileChooserButton *filechooserbutton, gpointer data) ;
gboolean on_mouse_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data) ;
GdkPixbuf* convertImageToPixbuf(Image image) ;


#endif