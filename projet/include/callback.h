#ifndef CALLBACK_H
#define CALLBACK_H

#include <gtk/gtk.h>

#include "image.h"

typedef struct {
    GdkPixbuf *originalPixbuf;  // L'image d'origine non zoomée
    GdkPixbuf *displayedPixbuf; // L'image affichée, pouvant être zoomée et déplacée
    int offsetX;                // Décalage horizontal de l'affichage par rapport à l'image d'origine
    int offsetY;                // Décalage vertical de l'affichage par rapport à l'image d'origine
} ImageInfo;

void open_file(GtkMenuItem *menu_item, gpointer user_data);
gboolean on_mouse_button_release(GtkWidget *widget, GdkEventButton *event, GtkWidget *image);
void on_zoom_in_clicked(GtkButton *button, gpointer user_data); 
void on_zoom_out_clicked(GtkButton *button, gpointer user_data); 
void update_button_state(GtkButton *button, gboolean is_clicked);
GdkPixbuf* convertImageToPixbuf(Image image);
Image convertPixbufToImage(GdkPixbuf *pixbuf); 
#endif /* CALL_BACK_H */