#ifndef CALLBACK_H
#define CALLBACK_H

#include <gtk/gtk.h>

#include "image.h"
#include "interpolation.h"
#include "gui_f.h"



typedef struct {
    ZoomType type;
    float zoomFactor;
    Zoom zoomFunc;
    Image *resultImage;
} ThreadArgs;


void open_file(GtkMenuItem *menu_item, gpointer user_data);
gboolean on_mouse_button_release(GtkWidget *widget, GdkEventButton *event, GtkWidget *image); 
void on_zoom_in_clicked(GtkButton *button, gpointer user_data); 
void on_zoom_out_clicked(GtkButton *button, gpointer user_data); 
void update_button_state(GtkButton *button, gboolean is_clicked);
GdkPixbuf* convertImageToPixbuf(Image image);
void on_click_quit(GtkWidget *button, gpointer window);
Image convertPixbufToImage(GdkPixbuf *pixbuf); 
void on_combo_box_changed(GtkComboBox *combo_box, gpointer user_data);
void save_file(GtkMenuItem *menu_item, gpointer user_data); 
gboolean draw_histogram(GtkWidget *widget, cairo_t *cr, gpointer data);
void *timed_zoom_thread(void *args); 
void timed_zoom(ZoomType type, float zoomFactor, Zoom zoomFunc);


#endif