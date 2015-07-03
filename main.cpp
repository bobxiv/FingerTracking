#include <iostream>
#include <algorithm>
#include <cmath>

//#include <omp.h>
#include "DemoApp.h"

#include "Esqueleto.h"

#include <stdlib.h>
#include <GL/glut.h>


using namespace std;

#define CONSOLE_VERBOSE

int current_state = INICIO;

int g_argc;
char **g_argv;
Esqueleto* mano;
int mainWindow(0), freenectWindow(0), demoWindow(0);


void estadoInicialDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);
	mano->DibujarRaw( );// solo profundidad
						// Negro no se sabe la profundidad
						// Rojo cerca
						// Verde medio
						// Azul lejos
	glutSwapBuffers();
}

void estadoCalibracionDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);
	mano->DibujarMovimiento( );
	glutSwapBuffers();
}

void estadoManoDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);
	mano->DibujarMano( true );
	glutSwapBuffers();
}

void estadoEsqueletoDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);
	mano->DibujarMano( false );
	mano->DibujarEsqueleto();
	glutSwapBuffers();
}

void SetState() 
{
	int prevWindow = glutGetWindow();
	switch( current_state ) {
	case INICIO:
		glutDisplayFunc(&estadoInicialDisplay);
		glutSetWindow(mainWindow);
		glutSetWindowTitle("PDI - TP Final: INICIO");
		//glutIdleFunc(&estadoInicial);
		break;
	case CALIBRACION:
		glutDisplayFunc(&estadoCalibracionDisplay);
		glutSetWindow(mainWindow);
		glutSetWindowTitle("PDI - TP Final: CALIBRACION");
		//glutIdleFunc(&estadoCalibracion);
		break;
	case MANO_DETECTADA:
		//mano->profundidadCorte = 0.26;// PRUEBA!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		glutDisplayFunc(&estadoManoDisplay);
		glutSetWindow(mainWindow);
		glutSetWindowTitle("PDI - TP Final: MANO_DETECTADA");
		//glutIdleFunc(&estadoMano);
		break;
	case ESQUELETO_DETECTADO:
		glutDisplayFunc(&estadoEsqueletoDisplay);
		glutSetWindow(mainWindow);
		glutSetWindowTitle("PDI - TP Final: ESQUELETO_DETECTADO");
		//glutIdleFunc(&estadoEsqueleto);
		break;
	case DEDOS_DETECTADOS:
		glutDisplayFunc(&estadoInicialDisplay);
		glutSetWindow(mainWindow);
		glutSetWindowTitle("PDI - TP Final: DEDOS_DETECTADOS");
		//glutIdleFunc(&estadoInicial);
		break;
	case APLICACION_CORRIENDO:
		glutDisplayFunc(&estadoInicialDisplay);
		glutSetWindow(mainWindow);
		glutSetWindowTitle("PDI - TP Final: APLICACION_CORRIENDO");
		//glutIdleFunc(&estadoInicial);
		break;
	}
	glutSetWindow(prevWindow);
}

void freenectIdleFunc()
{
	switch( current_state )
	{
	case INICIO:
		mano->Actualizar( false, false, false );
		break;
	case CALIBRACION:
		mano->Actualizar( false, false, true );
		mano->CalibrarProfundidad();
		break;
	case MANO_DETECTADA:
		mano->Actualizar( true, false, false );
		break;
	case ESQUELETO_DETECTADO:
		mano->Actualizar( true, true, false );
		break;
	case DEDOS_DETECTADOS:
		mano->Actualizar( false, false, false );
		break;
	case APLICACION_CORRIENDO:
		mano->Actualizar( false, false, false );
		break;
	}
}

void mainWindow_IdleCallback()
{
	static unsigned int lt=0;
	int dt = glutGet(GLUT_ELAPSED_TIME) - lt;

	DemoApp &myApp = *DemoApp::GetInstance();

	if( myApp.isFrameThreshold(dt) )
	{
		lt = glutGet(GLUT_ELAPSED_TIME);

		//Update DemoApp
		myApp.Update();

		//Update FreenectApp
		freenectIdleFunc();

		glutPostWindowRedisplay(freenectWindow);
		glutPostWindowRedisplay(mainWindow);
		glutPostWindowRedisplay(demoWindow);
		
		//glFlush();
   		//glutSwapBuffers();
	}
}

void mainWindow_RenderCallback()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

void keyPressed(unsigned char key, int x, int y) {
	if (key == 27) //ESC
	{
		glutDestroyWindow(mainWindow);
	} else if (key == 'w') //Subir angulo de motor kinect
	{
		mano->SubirAngulo();
	} else if (key == 's') //Bajar angulo de motor kinect
	{
		mano->BajarAngulo();
	} else if (key == 'm' && current_state != APLICACION_CORRIENDO) //Pasar al estado siguient
	{
		++current_state;
		SetState();
	} else if (key == 'n' && current_state != INICIO) //Pasar al estado anterior
	{
		current_state--;
		SetState();
	}
}

void ReSizeGLScene(int Width, int Height)
{
	glViewport(0,0,Width,Height);
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho (0, w, h, 0, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
	mano = Esqueleto::GetInstance();
	//glutInit(&g_argc, g_argv);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	//glutInitWindowSize(w, h);
	glutInitWindowSize(800+640, 600);
	glutInitWindowPosition(0, 0);
	//Main window
	mainWindow = glutCreateWindow("PDI - TP Final");

	//Freenect subwindow
	freenectWindow = glutCreateSubWindow(mainWindow, 800, 0, 640, 480);
	glutSetWindow(freenectWindow);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glutReshapeFunc(&ReSizeGLScene);
	glutKeyboardFunc(&keyPressed);
	SetState();
	ReSizeGLScene(w, h);
	
	//Demo subwindow
	demoWindow = glutCreateSubWindow(mainWindow, 0, 0, 800, 600);
	glutSetWindow(demoWindow);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	DemoApp::GetInstance()->Initialize(demoWindow, 800, 600, 60);

	glutSetWindow(mainWindow);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glutIdleFunc(&mainWindow_IdleCallback);
	glutDisplayFunc(&mainWindow_RenderCallback);

	glutMainLoop();
	/*
	omp_set_num_threads(2);
	int i;
	#pragma omp parallel private(i)
	{
		#pragma omp for
		for(i=0; i < 2 ; ++i)
		{
			if( omp_get_num_threads() == 1 )
				int u=2;//glutMainLoop();
			else
				DemoApp::GetInstance()->Go(800,600,60);
		}
	}
	*/
	return 0;
}
