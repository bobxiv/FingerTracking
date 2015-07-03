#include "Esqueleto.h"

#include <cassert>

Esqueleto::Esqueleto() : 
	freenect(),
	device(&freenect.createDevice<MyFreenectDevice>(0)),
	freenect_angle(0),
	requested_format(FREENECT_VIDEO_RGB),
	centroX(w/2),
	centroY(h/2),
	profundidadMano(0.1),
	angulo(90)
	{
	device->startDepth();
	for(int i=0; i<ndirs; i++)
		anguloDedo[i] = 120 + i * 15;
	for(int i=0; i<h; ++i) {
		for(int j=0; j<w; ++j) {
			depth[i][j]=0;
			lastDepth[i][j]=0;
			mask[i][j]=0;
		}
	}
	profundidadCorte=1;
}

Esqueleto::~Esqueleto() {
	device->stopDepth();
}

void Esqueleto::DibujarRaw ( ) {
	double max=0, min=1, r, g, b;
	glBegin(GL_POINTS);
	for(int i=0; i<h; ++i) {
		for(int j=0; j<w; ++j) {
			double v = depth[i][j];
			if ( v < 0 ) {
				r = 0;
				g = 0;
				b = 0;
			}
			else if( v <= 0.5) {
				r = 1 - 2*v;
				g = 2*v;
				b = 0;
			}
			else {
				r = 0;
				g = 1 - 2*(v-0.5);
				b = 2*(v-0.5);
			}
			glColor3d( r, g, b );
			glVertex2i( j, i );
			if( max < depth[i][j] ) max = depth[i][j];
			if( min > depth[i][j] && depth[i][j] >= 0 ) min = depth[i][j];
		}
	}
	//cout<<"max: "<<max<<"         "<<"min: "<<min<<endl;
	glEnd();
}

void Esqueleto::DibujarMovimiento ( ) {
	glBegin(GL_POINTS);
	double r, g, b;
	for(int i=0; i<h; ++i) {
		for(int j=0; j<w; ++j) {
			r=g=b=0;
			if( depth[i][j] < 0 ) continue;
			if( mask[i][j] > 0 ) r = depth[i][j];
			if( depth[i][j] < profundidadCorte ) g = depth[i][j]; 
			glColor3d( r, g, b );
			glVertex2i( j, i );
		}
	}
	glEnd();
	//cout<<"profundidad limite: "<<profundidadCorte<<endl;
}

void Esqueleto::DibujarMano(bool contorno ) {
	static double contourn[h][w];
	if( contorno ) {
		for(int i=0; i<h; ++i) {
			for(int j=0; j<w; ++j) {
				contourn[i][j] = depth[i][j];
			}
		}
		FiltroPasaAlto(contourn);
	}
	glBegin( GL_POINTS );
	for(int i=0; i<h; ++i) {
		for(int j=0; j<w; ++j) {
			if(depth[i][j] < 0) continue;
			if(contorno) {
				double green = (contourn[i][j] > .1? 1. : 0);
				glColor3d( depth[i][j], depth[i][j] + green, depth[i][j]);
			}
			else
				glColor3d( depth[i][j], depth[i][j], depth[i][j]);
			glVertex2i( j, i );
		}
	}
	glEnd();
}


void Esqueleto::DibujarEsqueleto ( ) {
	glColor3d(0., 0., 1.);
	glPointSize(1);
	glBegin(GL_LINES);
	for(int k = 0; k < ndirs; ++k) {
		double ux = cos(M_PI*(anguloDedo[k]+angulo)/180.), uy = sin(M_PI*(anguloDedo[k]+angulo)/180.);
		glVertex2i(centroX, centroY);
		glVertex2i(centroX+ux*largoDedos[k]*largoDedoMax, centroY+uy*largoDedos[k]*largoDedoMax);
	}
	glColor3d(1.0, 1.0, 0.0);
	double ux = cos(M_PI*angulo/180.), uy = sin(M_PI*angulo/180.);
	glVertex2i(centroX, centroY);
	glVertex2i(centroX+ux*127, centroY+uy*127);
	glEnd();
}

void Esqueleto::CalibrarProfundidad ( ) {
	static vector<double> v;
	v.clear();
	//Calculo la mediana de los pixels enmascarados
	for(int i=0; i<h; ++i) {
		for(int j=0; j<w; ++j) {
			if( mask[i][j] > 0 )
				v.push_back( min(depth[i][j], lastDepth[i][j]) );
		}
	}
	if( v.size() > 10000 ) {
		sort(v.begin(), v.end());
		//Utilizo la mediana desplazada un "poco" hacia atras
		double mediana = v[ v.size()/2 ] + 0.02;
		profundidadCorte = profundidadCorte * 0.8 + mediana * 0.2;
	}
}

void Esqueleto::SubirAngulo ( ) {
	freenect_angle += 0.1;
	if (freenect_angle > 30) 
		freenect_angle = 30;
	device->setTiltDegrees(freenect_angle);
}

void Esqueleto::BajarAngulo ( ) {
	freenect_angle -= 0.1;
	if (freenect_angle < -30)
		freenect_angle = -30;
	device->setTiltDegrees(freenect_angle);
}

Esqueleto* Esqueleto::GetInstance()
{
	static Esqueleto mano;
	return &mano;
}

int Esqueleto::GetNumeroDedos()
{
	return ndirs;
}

void Esqueleto::GetPosicionPuntaDedo(size_t nDedo, float &x, float &y, float &z)
{
	if( nDedo >= (ndirs+1) )// if invalid value
	{
		x=std::numeric_limits<float>::infinity();
		y=std::numeric_limits<float>::infinity();
		z=std::numeric_limits<float>::infinity();
		return;
	}

	if( nDedo == ndirs )// if hand center is wanted
	{
		freenect_resolution resolution = device->getDepthResolution();
		switch(resolution)
		{
			case FREENECT_RESOLUTION_LOW: /**< QVGA - 320x240 */
			{
				x = centroX /320;
				y = centroY /240;
			}
			break;
			case FREENECT_RESOLUTION_MEDIUM: /**< VGA  - 640x480 */
			{
				x = centroX /640;
				y = centroY /480;
			}
			break;
			case FREENECT_RESOLUTION_HIGH: /**< SXGA - 1280x1024 */
			{
				x = centroX /1280;
				y = centroY /1024;
			}
			break;
			default:
				assert(false);
		}

		float _z = Helper(static_cast<int>(centroY), static_cast<int>(centroX));

		//z = profundidadCorte-depth[static_cast<int>(centroX)][static_cast<int>(centroY)];
		//z = 1.0-depth[static_cast<int>(centroX)][static_cast<int>(centroY)];
		z = max( profundidadCorte-_z, 0.0f );
		z = z/profundidadCorte;
		z = sqrt(z);
		std::cerr<<"Z: "<<z<<std::endl;
	}else//if finger is wanted
	{

		double anguloFromCenter = angulo + anguloDedo[nDedo];
		freenect_resolution resolution = device->getDepthResolution();
		switch(resolution)
		{
			case FREENECT_RESOLUTION_LOW: /**< QVGA - 320x240 */
			{
				x = ( centroX + largoDedoMax*largoDedos[nDedo]*cos(anguloFromCenter/180*M_PI) ) /320; 
				y = ( centroY + largoDedoMax*largoDedos[nDedo]*sin(anguloFromCenter/180*M_PI) ) /240; 
			}
			break;
			case FREENECT_RESOLUTION_MEDIUM: /**< VGA  - 640x480 */
			{
				x = ( centroX + largoDedoMax*largoDedos[nDedo]*cos(anguloFromCenter/180*M_PI) ) /640; 
				y = ( centroY + largoDedoMax*largoDedos[nDedo]*sin(anguloFromCenter/180*M_PI) ) /480; 
			}
			break;
			case FREENECT_RESOLUTION_HIGH: /**< SXGA - 1280x1024 */
			{
				x = ( centroX + largoDedoMax*largoDedos[nDedo]*cos(anguloFromCenter/180*M_PI) ) /1280; 
				y = ( centroY + largoDedoMax*largoDedos[nDedo]*sin(anguloFromCenter/180*M_PI) ) /1024; 
			}
			break;
			default:
				assert(false);
		}

		float _z = Helper(static_cast<int>(static_cast<int>(centroY + largoDedos[nDedo]*sin(anguloFromCenter/180*M_PI))), static_cast<int>(centroX + largoDedos[nDedo]*cos(anguloFromCenter/180*M_PI)));

		//z = 1.0-depth[static_cast<int>(centroX + largoDedos[nDedo]*cos(anguloFromCenter/180*M_PI))][static_cast<int>(centroY + largoDedos[nDedo]*sin(anguloFromCenter/180*M_PI))];
		//std::cerr<<(depth[static_cast<int>(centroX + largoDedos[nDedo]*cos(anguloFromCenter/180*M_PI))][static_cast<int>(centroY + largoDedos[nDedo]*sin(anguloFromCenter/180*M_PI))] > 0)<<std::endl;
		//std::cerr<<(depth[static_cast<int>(centroX + largoDedos[nDedo]*cos(anguloFromCenter/180*M_PI))][static_cast<int>(centroY + largoDedos[nDedo]*sin(anguloFromCenter/180*M_PI))] > profundidadCorte)<<std::endl;
		//z = profundidadCorte-depth[static_cast<int>(centroX + largoDedos[nDedo]*cos(anguloFromCenter/180*M_PI))][static_cast<int>(centroY + largoDedos[nDedo]*sin(anguloFromCenter/180*M_PI))];
		z = max( profundidadCorte-_z, 0.0f );
		z = z/profundidadCorte;
		z = sqrt(z);
		//std::cerr<<"X: "<<x<<"Y: "<<y<<"Z: "<<z<<std::endl;
	}
}

float Esqueleto::Helper(int _x, int _y)
{
	if(_x < 0 || _x > 479 || _y < 0 || _y > 639 )
		return 0;
	int _w = 16;
	int _h= 16;
	float* z = new float[_w*_h];
	for(size_t i=0; i < _h ;++i)
	{
		for(size_t j=0; j < _w ;++j)
		{
			int id = _x-_h/2+i;
			int jd = _y-_w/2+j;
			if(jd < 0 || jd > 639 || id < 0 || id > 479 )
				z[j+i*_w] = 0;
			else
				z[j+i*_w] = depth[id][jd];
		}
	}
	float sum = 0;
	int num = 0;
	for(size_t g=0; g < (_w*_h) ;++g)
	{
		sum += (z[g]>0)? z[g] : 0;
		num += (z[g]>0)? 1 : 0;
	}
	if( num > 0 )
		sum /= num;
	else
		sum = profundidadCorte;
	return sum;
}