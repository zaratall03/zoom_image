
#include <stdlib.h>
#include "image.h"
#include "stb_image.h"  
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



#define DEFAULT_QUALITY 95
#define MAX_CHANNELS 4


/**
 * Les fonctions correspondent aux primitives de la structure `Image`
 * 
 * @param image La structure ` Image ` contient généralement des informations sur une image, telles que
 * sa largeur, sa hauteur, le nombre de canaux, les données de pixels et le chemin du fichier. Les
 * fonctions fournies sont utilisées pour récupérer des informations spécifiques à partir d'un objet
 * `Image`.
 * 
 * @return Les fonctions renvoient diverses propriétés d'un objet `Image` :
 * - `getWidth` : Renvoie la largeur de l'image.
 * - `getHeight` : Renvoie la hauteur de l'image.
 * - `getnbPixelChannels` : Renvoie le nombre de canaux dans l'image.
 * - `getData` : Renvoie les données de l'image.
 * - `getImagePath` : Renvoie le chemin de l'image.
 */
int getWidth(Image image){
    return image.width;
}
int getHeight(Image image){
    return image.height;
}

int getnbPixelChannels(Image image){
    return image.channels;
}

char* getData(Image image){
    return image.data;
}

char* getImagePath(Image image){
    return image.path;
}


/**
 * La fonction getPixel récupère les valeurs des pixels à une position spécifiée dans une image.
 * 
 * @param image Le paramètre ` image ` représente une structure de données d'image qui contient des
 * informations sur l'image.
 * @param x Le paramètre ` x ` représente la coordonnée x du pixel dans l'image.
 * @param y Le paramètre 'y' représente la position verticale du pixel dans l'image.
 * 
 * @return Cette fonction renvoie un pointeur vers un tableau de caractères non signé représentant les
 * valeurs de pixels aux coordonnées spécifiées (x, y) dans la structure Image donnée.
 */
unsigned char* getPixel(Image image, int x, int y) {
    unsigned char * pixel = (unsigned char*)malloc(sizeof(unsigned char ) * image.channels);
    if (pixel == NULL) {
        printf("Erreur lors de l'allocation de mémoire pour le pixel.\n");
        return NULL;
    } 
    for (int channel = 0; channel < image.channels; channel++) {
        int index = (y * image.width + x) * image.channels + channel;
        pixel[channel] = abs(image.data[index]);
    }
    return pixel;
}


/**
 * Définit la valeur d'un pixel dans une image aux coordonnées (x, y) et dans le canal spécifié.
 * 
 * @param img       Pointeur vers une structure Image représentant l'image.
 * @param x         Coordonnée horizontale du pixel à définir.
 * @param y         Coordonnée verticale du pixel à définir.
 * @param c         Canal de couleur du pixel à définir.
 * @param value     Intensité ou valeur de couleur à définir pour le pixel.
 */
void set_pixel(Image *img, int x, int y, int c, unsigned char value) {
    img->data[(y * img->width + x) * img->channels + c] = value;
}

/**
 * Charge une image à partir d'un chemin spécifié et renvoie une structure Image.
 * 
 * @param path Le chemin du fichier de l'image à charger.
 * 
 * @return  Une structure Image contenant les informations de l'image chargée.
 */
Image loadImage(char* path) {
    char *image_path = path;
    int width, height, channels;
    unsigned char *image_data = stbi_load(image_path, &width, &height, &channels, STBI_rgb);
    if (!image_data) {
        perror("Erreur lors du chargement de l'image.\n");
        exit(1);
    }
    
    Image img = {width, height, channels, image_data, image_path};
    return img;
}

/**
 * Affiche les valeurs des pixels à une position donnée dans une image.
 * 
 * @param image     L'image à partir de laquelle les valeurs de pixels seront affichées.
 * @param x         Coordonnée horizontale du pixel à afficher.
 * @param y         Coordonnée verticale du pixel à afficher.
 */
void affichePixel(Image image, int x, int y){
    unsigned char* pixels = getPixel(image, x, y);
    int channels =  getnbPixelChannels(image);
    printf("[");
    for(int i=0; i<channels; i++){
        printf(" %c ", *(pixels+i)); 
    }
    printf("]");
}

/**
 * La fonction `freeImage` libère la mémoire allouée à une image et renvoie une structure d'image vide.
 * 
 * @param image Le paramètre `image` dans la fonction `freeImage` est de type `Image`.
 * 
 * @return La fonction `freeImage` renvoie une structure `Image` avec tous ses champs initialisés à
 * zéro ou NULL.
 */
Image freeImage(Image image){
    stbi_image_free(image.data);
    Image img = {0, 0,0, NULL, NULL};
    return img;
}

/**
 * Libère la mémoire allouée pour une structure Image, y compris ses données et son chemin.
 * 
 * @param image Pointeur vers la structure Image à libérer.
 */
void free_image(Image *image) {
    if (image != NULL) {
        if (image->data != NULL) {
            free(image->data);
        }
        if (image->path != NULL){
            free(image->path);
        }
    }
}
/**
 * Récupère la valeur d'un canal de couleur spécifique pour un pixel dans une image.
 * 
 * @param image     La structure de données d'image contenant les pixels.
 * @param x         Coordonnée horizontale du pixel.
 * @param y         Coordonnée verticale du pixel.
 * @param channel   Le canal de couleur spécifique du pixel.
 * 
 * @return  La valeur du composant de canal du pixel aux coordonnées (x, y).
 */
unsigned char getPixelComposante(Image image, int x, int y, int channel) {
    int index = (y * image.width + x) * image.channels + channel;
    return image.data[index];
}

/**
 * Définit la valeur d'un canal spécifique pour un pixel dans une image.
 * 
 * @param image     Structure de données représentant l'image.
 * @param x         Coordonnée horizontale du pixel.
 * @param y         Coordonnée verticale du pixel.
 * @param channel   Canal de couleur du pixel.
 * @param value     Nouvelle valeur pour le canal spécifié du pixel.
 */
void setPixelComposante(Image *image, int x, int y, int channel, unsigned char value) {
    if (x < 0 || x >= image->width || y < 0 || y >= image->height || channel < 0 || channel >= image->channels) {
        fprintf(stderr, "SET Coordonnées de pixel invalides\n");
        exit(EXIT_FAILURE);
    }
    int index = (y * image->width + x) * image->channels + channel;
    image->data[index] = value;
}


/**
 * Écrit une image au format PNG dans un fichier spécifié.
 * 
 * @param filename  Nom du fichier où l'image sera enregistrée.
 * @param image     Objet image à écrire dans le fichier.
 */
void writeImageJpg(const char* filename, Image image){ 
    int result = stbi_write_png(filename, image.width, image.height, image.channels, image.data, image.width * image.channels);
    if (!result) {
        fprintf(stderr, "Erreur lors de l'écriture de l'image PNG : %s\n", filename);
        exit(EXIT_FAILURE);
    }
}



