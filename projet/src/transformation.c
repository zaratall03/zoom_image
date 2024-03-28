#include "transformation.h"
#include "image.h"

void increaseBrightness(Image *image, int amount) {
    for (int i = 0; i < image->width * image->height; i++) {
        int newValue = (int)image->data[i] + amount;
        newValue = newValue < 0 ? 0 : (newValue > 255 ? 255 : newValue);
        image->data[i] = (unsigned char)newValue;
    }
}