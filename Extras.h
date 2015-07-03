#ifndef EXTRAS_H
#define EXTRAS_H

const int w = 640, h = 480;

void FiltroPasaAlto(double img[h][w]);
void FiltroPasaBajo(double img[h][w]);
//void k_means(double angs[], double corrs[], size_t nvals, double means[], size_t nmeans);
double Maximo( double vector[], int n );
#endif

