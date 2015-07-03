#include "Esqueleto.h"
#include <CImg.h>

using namespace cimg_library;

void Esqueleto::Actualizar( bool profundidad, bool calcEsq, bool marcMov ) {
	if( !device->HayNuevoFrame() ) return;
	profundidad |= calcEsq;
	if( marcMov ) {
		// Guardo el frame anterior
		for(int i=0; i<h; ++i) {
			for(int j=0; j<w; ++j) {
				lastDepth[i][j] = depth[i][j];
			}
		}
	}
	// Busco un frame nuevo
	device->updateState();
	device->getDepth(depth);
	// Enmascaro movimientos, para calibrar la profundidad
	if( marcMov ) {
		for(int i=0; i<h; ++i) {
			for(int j=0; j<w; ++j) {
				double &a = depth[i][j], &b = lastDepth[i][j];
				if(a > 0 && b > 0 &&  a < 0.5 && b < 0.5 && ((a-b > 0.02) || (b-a > 0.02)) )
					mask[i][j] = 1;
				else if( mask[i][j] > 0)
					mask[i][j]--;
			}
		}
	}
	double cuantoHay = 0;
	// Elimino lo que queda detras de la profundidad limite
	if( profundidad ) {
		for(int i=0; i<h; ++i) {
			for(int j=0; j<w; ++j) {
				if( depth[i][j] > profundidadCorte )
					depth[i][j] = -1;
				else if( depth[i][j] > 0 )
					cuantoHay += depth[i][j];
			}
		}
	}
	// Realizo los calculos
	if(calcEsq && cuantoHay > 1) {
		ActualizarCentro( );
		ActualizarDedos( );
		ActualizarAngulo( );
	}
}

void Esqueleto::ActualizarCentro ( ) {
	double accum = 0, nxc=0, nyc=0;
	for(int i=0; i<h; ++i) {
		for(int j=0; j<w; ++j) {
			if(depth[i][j] >= 0 && depth[i][j] < profundidadCorte){
				nxc += j;
				nyc += i;
				accum++;
			}
		}
	}
	if(accum != 0) {
		centroX = (centroX + (nxc / accum + cos(M_PI*angulo/180.) * 30))/2.0;
		centroY = (centroY + (nyc / accum + sin(M_PI*angulo/180.) * 30))/2.0;
	}
}


void Esqueleto::ActualizarDedos( ) {
	double corr[360] = {0};
	double ld[360] = {0};
	double newAnguloDedo[ndirs];
	for(int i=0;i<ndirs;i++) { newAnguloDedo[i]=anguloDedo[i]; }
	for(int i=90; i<270;i++){
		double ux = cos(M_PI*(i+angulo)/180.), uy = sin(M_PI*(i+angulo)/180.);
		for(int j=0; j<largoDedoMax; ++j){
			int a = centroX + ux*j;
			int b = centroY + uy*j;
			if(a>0 && a<w && b>0 && b<h && depth[b][a]>=0)
				corr[i] += double(j) / (depth[b][a]+.01);
			if( depth[b][a] > 0) ld[i] = j;
		}
	}
	for(int i=90; i<270;i++)
		ld[i] /= double(largoDedoMax);
	double corte = Maximo( corr, 360 ) * 0.2;
	if( corte == 0 ) return;
	
	int primero=-1, ultimo=0;
	double media=0, suma=0;
	
	for(int i=0;i< 360 ;i++) {
		corr[i] = max( 0.0, corr[i]-corte );
		if( corr[i] != 0 ) {
			media += corr[i] * i;
			suma += corr[i];
			if( primero == -1 )
				primero = i;
			else
				ultimo = i;
		}
	}
	media /= suma;
	static const double manoPorDefecto[ndirs] = {0, 0.4, 0.6, 0.8, 1.0};
	if(media < (ultimo-primero)/2 + primero){
//		cout<<"es zurdo\n";
		for(int i=0;i<ndirs;i++)
			newAnguloDedo[i] = primero + (ultimo-primero) * (1 - manoPorDefecto[i]);
	}
	else{
//		cout<<"es diestro\n";
		for(int i=0;i<ndirs;i++)
			newAnguloDedo[i] = primero + (ultimo-primero) * manoPorDefecto[i];
	}
	
	
	int contador=0;
	while( contador++ < 100 ) {
		int ini=0, fin=0;
		for( int dir=1; dir<=ndirs; dir++) {
			ini = fin;
			fin = ((dir!=ndirs)? (newAnguloDedo[dir-1] + newAnguloDedo[dir])/2.0 : 360);
			double media=0, acum=0;
			for(int i=ini; i<fin; i++) {
				media += i * corr[i];
				acum += corr[i];
			}
			if(acum > 0) {
				media /= acum;
				newAnguloDedo[dir-1] = int(media);
			}
		}
	}
	for(int i=0;i<ndirs;i++) { 
		anguloDedo[i] = (anguloDedo[i] + newAnguloDedo[i])/2;
		largoDedos[i] = (largoDedos[i] + ld[ (int)anguloDedo[i] ])/2;
	}
	
	CImg<double> img(360,360,1,3), corr_img(360);
	img.fill(0);
	for(int i=0;i<360;++i){
		corr_img(i) = corr[i/2 + 90];
	}
	const unsigned char green[] = {0, 255, 0};
	const unsigned char orange[] = {255,69,0}; 
	img.draw_graph(corr_img, orange);
	for(int i = 0; i < ndirs; ++i)
		img.draw_line(anguloDedo[i]*2 - 180, 0, anguloDedo[i]*2 - 180, 360, green);
	static CImgDisplay display(img, "Correlaciones");
	display.display(img);
}

void Esqueleto::ActualizarAngulo ( ) {
	vector<double> corr(360,0);
	double acumm = 0;
	for(int theta = 0; theta<360; ++theta){
		double ux = cos(M_PI*theta/180.), uy = sin(M_PI*theta/180.);
		for(int i=0; i<127; ++i){
			int a = centroX + ux*i;
			int b = centroY + uy*i;
			if(a>0 && a<w && b>0 && b<h && depth[b][a]>=0)
				corr[theta] += double(i)/(depth[b][a]+.01);
		}
		acumm += corr[theta];
	}
	if( acumm > 100 ) {
		double newAngulo = (max_element( corr.begin(), corr.end() ) - corr.begin()) + 180.0;
		angulo = 0.7*angulo + 0.3*newAngulo;
	}
}
