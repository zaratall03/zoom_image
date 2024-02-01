#include "interpolations.h"
#include <math.h>


void bilinear(unsigned char *image, unsigned char *imzoom, int r, int c, int zoom) {
    int i, j, ii, jj, near_i, near_j;
    float x, y, dx, dy, fx, fy, value;

    for (i = 0; i < r*zoom; i++) {
        for (j = 0; j < c*zoom; j++) {
            x = j/zoom;
            y = i/zoom;
            near_i = (int)floor(y);
            near_j = (int)floor(x);
            dx = x - near_j;
            dy = y - near_i;
            ii = near_i*c;
            jj = near_j;
            fx = 1 - dx;
            fy = 1 - dy;
            value = fx*(fy*image[ii + jj] + dy*image[ii + jj + c]) +
                    dx*(fy*image[ii + jj + 1] + dy*image[ii + jj + c + 1]);
            imzoom[i*c*zoom + j] = (unsigned char)value;
        }
    }
}

float hermite(float p0, float m0, float p1, float m1, float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    float a = 2.0 * t3 - 3.0 * t2 + 1.0;
    float b = t3 - 2.0 * t2 + t;
    float c = -2.0 * t3 + 3.0 * t2;
    float d = t3 - t2;
    return a * p0 + b * m0 + c * p1 + d * m1;
}


void hermiteZoom(unsigned char *image, unsigned char *imzoom, int r, int c, int zoom) {
    int i, j, ii, jj, near_i, near_j;
    float x, y, dx, dy;

    for (i = 0; i < r * zoom; i++) {
        for (j = 0; j < c * zoom; j++) {
            x = j / (float)zoom;
            y = i / (float)zoom;
            near_i = (int)floor(y);
            near_j = (int)floor(x);
            dx = x - near_j;
            dy = y - near_i;

            int i0 = near_i > 0 ? near_i - 1 : 0;
            int i1 = near_i;
            int i2 = near_i < r - 1 ? near_i + 1 : r - 1;
            int i3 = near_i < r - 2 ? near_i + 2 : r - 1;

            int j0 = near_j > 0 ? near_j - 1 : 0;
            int j1 = near_j;
            int j2 = near_j < c - 1 ? near_j + 1 : c - 1;
            int j3 = near_j < c - 2 ? near_j + 2 : c - 1;

            float p0 = hermite(image[i0 * c + j0], (image[i1 * c + j0] - image[i0 * c + j0]) / 2.0, image[i2 * c + j0], (image[i2 * c + j0] - image[i1 * c + j0]) / 2.0, dy);
            float p1 = hermite(image[i0 * c + j1], (image[i1 * c + j1] - image[i0 * c + j1]) / 2.0, image[i2 * c + j1], (image[i2 * c + j1] - image[i1 * c + j1]) / 2.0, dy);
            float p2 = hermite(image[i0 * c + j2], (image[i1 * c + j2] - image[i0 * c + j2]) / 2.0, image[i2 * c + j2], (image[i2 * c + j2] - image[i1 * c + j2]) / 2.0, dy);
            float p3 = hermite(image[i0 * c + j3], (image[i1 * c + j3] - image[i0 * c + j3]) / 2.0, image[i2 * c + j3], (image[i2 * c + j3] - image[i1 * c + j3]) / 2.0, dy);

            float result = hermite(p0, (p1 - p0) / 2.0, p2, (p2 - p1) / 2.0, dx);
            imzoom[i * c * zoom + j] = (unsigned char)result;
        }
    }
}

void nnbr(unsigned char *image, unsigned char *imzoom, int r, int c, int zoom) {
    int i, j, near_i, near_j;
    for (i = 0; i < r * zoom; i++) {
        for (j = 0; j < c * zoom; j++) {
            near_i = (int)floor(i / zoom);
            near_j = (int)floor(j / zoom);
            imzoom[i * c * zoom + j] = image[near_i * c + near_j];
        }
    }
}
