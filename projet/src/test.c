#include "callback.h"
#include "gui_f.h"
#include "image.h"

Image imageSrc; // Variable globale pour stocker l'image originale
Image zoomedImage; // Variable globale pour stocker l'image zoomée
GtkWidget *image; // Variable globale pour le widget GtkImage
MouseCallbackData *callbackData;
GtkWidget *window; 
GtkWidget *file_chooser_button;
GtkWidget *combobox; 
GtkWidget *vbox;


int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    init_main_window();
    init_image_displayer();
    init_image_chooser();
    init_zoom_type_chooser();
    
    pack_all();

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}