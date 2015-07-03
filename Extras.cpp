
#include "Extras.h"
#include <cmath>
using namespace std;
void FiltroPasaAlto(double img[h][w]) {
	const int mask_w = 3, mask_h = 3; //OJO con las mascaras con alguna dimension par
	double mask[mask_h][mask_w] = { {-0.5, -1, -0.5} , {-1, 6, -1} , {-0.5, -1, -0.5} };
	
	double res[h+mask_h-1][w+mask_w-1];
	
	for(int i=0; i<h+mask_h-1; ++i) {
		for(int j=0; j<w+mask_w-1; ++j) {
			res[i][j] = 0;
			for(int k=0; k<mask_h; ++k) {
				for(int l=0; l<mask_w; ++l) {
					if(i-k<0 || i-k>=h || j-l<0 ||j-l>=w)
						continue;
					res[i][j] +=  mask[k][l]*img[i-k][j-l];
				}
			}
		}
	}
	
	for(int i=mask_h/2; i<h+mask_h/2; ++i) {
		for(int j=mask_w/2; j<w+mask_w/2; ++j) {
			img[i-mask_h/2][j-mask_w/2] = res[i][j];
		}
	}
}

void FiltroPasaBajo(double img[h][w]) {
	const int mask_w = 3, mask_h = 3; //OJO con las mascaras con alguna dimension par
	double mask[mask_h][mask_w] = { {.0625, .125, .0625} , {.125, .25, .125} , {.0625, .125, .0625} };
	double res[h+mask_h-1][w+mask_w-1];
	for(int i=0; i<h+mask_h-1; ++i) {
		for(int j=0; j<w+mask_w-1; ++j) {
			res[i][j] = 0;
			double count=0;
			for(int k=0; k<mask_h; ++k) {
				for(int l=0; l<mask_w; ++l) {
					if(i-k<0 || i-k>=h || j-l<0 || j-l>=w || img[i-k][j-l]<0)
						continue;
					count++;
					res[i][j] +=  mask[k][l]*img[i-k][j-l];
				}
			}
			if( count < 6 ) 
				res[i][j]=-1;
			else
				res[i][j] *= double(mask_h*mask_w)/count;
		}
	}
	for(int i=mask_h/2; i<h+mask_h/2; ++i) {
		for(int j=mask_w/2; j<w+mask_w/2; ++j) {
			img[i-mask_h/2][j-mask_w/2] = res[i][j];
		}
	}
}

//void k_means(double angs[], double corrs[], size_t nvals, double means[], size_t nmeans){
//	
//	int contador = 0;
//	
//	double corr_mean = 0;
//	for(int i = 0; i < nvals; ++i){
//		corr_mean += corrs[i];
//	}
//	corr_mean /= nvals;
//	
//	while(contador++ < 350){
//		
//		int pto_medio, pto_medio_ini = int(means[nmeans-1] + (360 - means[nmeans-1] + means[0]) / 2.) % 360;
//		int i = 0;
//		
//		double accum_numerador = 0;
//		double accum_denominador = 0;
//		if(pto_medio_ini > means[nmeans-1]){
//			for(i = pto_medio_ini; i < 360; ++i){
//				if(corrs[i] > 2*corr_mean){ 
//					accum_numerador += pow(corrs[i],6) * angs[i];
//					accum_denominador += pow(corrs[i],6);
//				}
//			}
//			i = 0;
//		}
//		
//		for(int k = 0; k < nmeans-1; ++k){
//			pto_medio = (means[k] + means[k+1]) / 2;
//			for(; i < pto_medio; ++i){
//				if(corrs[i] > 2*corr_mean){ 
//					accum_numerador += corrs[i] * angs[i];
//					accum_denominador += corrs[i];
//				}
//			}
//			if(accum_denominador != 0)
//				means[k] = accum_numerador / accum_denominador;
//			accum_numerador = accum_denominador = 0;
//		}
//		
//		if(pto_medio_ini > means[nmeans-1]){
//			for(; i < pto_medio_ini; ++i){
//				if(corrs[i] > 2*corr_mean){ 
//					accum_numerador += corrs[i] * angs[i];
//					accum_denominador += corrs[i];
//				}
//			}
//			if(accum_denominador != 0)
//				means[nmeans-1] = accum_numerador / accum_denominador;
//			accum_numerador = accum_denominador = 0;
//		}
//		else{
//			for(; i < 360; ++i){
//				if(corrs[i] > 2*corr_mean){ 
//					accum_numerador += corrs[i] * angs[i];
//					accum_denominador += corrs[i];
//				}
//			}
//			for(i = 0; i < pto_medio_ini; ++i){
//				if(corrs[i] > 2*corr_mean){ 
//					accum_numerador += corrs[i] * angs[i];
//					accum_denominador += corrs[i];
//				}
//			}
//			if(accum_denominador != 0)
//				means[nmeans-1] = accum_numerador / accum_denominador;
//			accum_numerador = accum_denominador = 0;
//		}
//	}
//	
//	for(int i = 0; i < nvals; ++i){
//		if(corrs[i] <= 2*corr_mean)
//			corrs[i] = 0;
//	}
//}

double Maximo(double v[], int n) {
	double max=v[0];
	for(int i=0;i< n ;i++) { 
		if( v[i] > max )
			max = v[i];
	}
	return max;
}
