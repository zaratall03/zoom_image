#ifndef INTERPOLATIONS_H
#define INTERPOLATIONS_H

void nnbr(unsigned char *image, unsigned char *imzoom, int r, int c, int zoom);
void hermiteZoom(unsigned char *image, unsigned char *imzoom, int r, int c, int zoom);
float hermite(float p0, float m0, float p1, float m1, float t);
void bilinear(unsigned char *image, unsigned char *imzoom, int r, int c, int zoom);

#endif