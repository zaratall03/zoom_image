#ifndef CALLBACK_H
#define CALLBACK_H

#include <gtk/gtk.h>

#include "interpolation.h"
#include "image.h"

typedef struct {
    ZoomType zoomType; // Type de zoom à effectuer
    gpointer data; // Autres données à passer si nécessaire
} MouseCallbackData;

void on_file_selection(GtkFileChooserButton *filechooserbutton, gpointer data);
gboolean on_mouse_button_press(GtkWidget *widget, GdkEventButton *event, gpointer userData);
void on_combobox_changed(GtkComboBox *combobox, gpointer user_data);

GdkPixbuf* convertImageToPixbuf(Image image);

#endif /* CALL_BACK_H */
