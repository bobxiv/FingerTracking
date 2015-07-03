#ifndef ESQUELETO_H
#define ESQUELETO_H
#define GLUT_BUILDING_LIB
#include <GL/glut.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#ifndef M_PI
#define M_PI 3.141592653589793
#endif
using namespace std;

#include "Extras.h"
#include "MyFreenectDevice.h"

// Numero de dedos
#define ndirs 5

// Cosas variables para calcular el angulo
#define distMinima 15
#define largoDedoMax 180.0

class Esqueleto {
//private:
public:
	Esqueleto();

	Freenect::Freenect freenect;
	MyFreenectDevice* device;
	double freenect_angle;
	freenect_video_format requested_format;
	
	double centroX;
	double centroY;
	double profundidadMano;
	double angulo;
	double anguloDedo[ndirs];
	double largoDedos[ndirs];
	
	double depth[h][w];
	double lastDepth[h][w];
	double mask[h][w];
	double profundidadCorte;
	
	void ActualizarCentro( );
	void ActualizarAngulo( );
	void ActualizarDedos( );
public:
	~Esqueleto();
	
	void CalibrarProfundidad();
	
	void Actualizar( bool EnmascararProfundidad = true, bool calcularEsqueleto = true, bool marcarMovimiento = false );
	
	void DibujarRaw( );
	void DibujarMovimiento( );
	void DibujarMano(bool contorno);
	void DibujarEsqueleto();
	
	void SubirAngulo();
	void BajarAngulo();

	static Esqueleto* GetInstance();

	int GetNumeroDedos();
	void GetPosicionPuntaDedo(size_t nDedo, float &x, float &y, float &z);// if nDedo is GetNumeroDedos() hand center is given
	float Helper(int _x, int _y);
	
};

#endif

