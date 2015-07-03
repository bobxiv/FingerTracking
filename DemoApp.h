#pragma once
#ifndef DEMOAPP_H
#define DEMOAPP_H

//Osea GLSL
#define GLEW_BUILD //Para linkear GLEW de forma estatica
#include <GL/glew.h>

#include <stdlib.h>
#include <GL/glut.h>
#include "Texture2D.h"

#include "VectorMath.h"

#include <map>
#include <string>

#include "Esqueleto.h"

#include <btBulletDynamicsCommon.h>

enum states {INICIO=0, CALIBRACION=1, MANO_DETECTADA=2, ESQUELETO_DETECTADO=3, DEDOS_DETECTADOS=4, APLICACION_CORRIENDO=5};
extern int current_state;

class DemoApp
{
private:

	int m_WindowH;
	int m_width, m_height;
	double m_milisecondsDelta;//miliseconds between frames

	btDiscreteDynamicsWorld* m_pDWorld;
    btSequentialImpulseConstraintSolver* m_pDSolver;
    btDefaultCollisionConfiguration* m_pCollisionConfiguration;
    btCollisionDispatcher* m_pDispatcher;
	btBroadphaseInterface* m_pBroadphase;

	btAlignedObjectArray<btCollisionShape*> m_apShapes;

	class Hand
	{
	public:
		btDynamicsWorld &m_DWorld;
		btRigidBody* m_pFingerPoints[6];// 6th is hand center and could be NULL
		btTransform m_Transforms[6];//5 finger points and 1 hand center
		GLUquadric* m_pFingerQuadric;
		GLuint m_GLDList;
		float m_radioEsferas;
		float m_handMinDepth;//minimum depth from ZNear!
		float m_handReach;//from m_handMinDepth!
		void GetFrustumAreaAtDepth(float zDepth, float &widthZDepth, float &heightZDepth);
		std::vector<float> m_PrevDepth;
	public:
		Hand(btDynamicsWorld* pDWorld, btRigidBody* pFingerPoints[6], float radioEsferas, float handMinDepth, float handReach);
		~Hand();
		void Update();
		void Render();
	};

	Hand* m_pHand;

	class Box
	{
	public:
		btDynamicsWorld &m_DWorld;
		btRigidBody* m_pBox;
		btTransform m_Transform;
		static CVector m_vQuad[12][3];
		static CVector2 m_vTexCoords[12][3];
		CVector m_TBNMatrix[12][3];
	public:
		static void Initialize();
		Box(btDynamicsWorld* pDWorld, btRigidBody* pBox);
		~Box();
		void Update();
		void Render(GLhandleARB hGLSLProgram, const CVector &vLightPos);
	};

	struct Camera
	{									//Right handed system
		btTransform m_FrameOfReference;	//base 1 left
										//base 2 up
		float x,y,z;					//base 3 forward

		float m_fov;
		float m_ratio;
		float m_ZNear;

		Camera( btVector3& Position, btVector3& Up, btVector3& LookAt, float fov, float ratio, float ZNear ): m_fov(fov), m_ratio(ratio), m_ZNear(ZNear)
		{
			Up.normalize();
			btVector3 forward = LookAt - Position;
			forward.normalize();
			btVector3 left = Up.cross(forward);
			btMatrix3x3 basis(	left.x(), Up.x(), forward.x(),
								left.y(), Up.y(), forward.y(),
								left.z(), Up.z(), forward.z() );
			m_FrameOfReference.setBasis(basis);
			m_FrameOfReference.setOrigin( Position );
		}

		void Move(float dx, float dy, float dz)
		{
			m_FrameOfReference.setOrigin(m_FrameOfReference.getOrigin()+GetLeft()*dx+GetUp()*dy+GetForward()*dz);
		}

		void Rotate(float dx, float dy, float dz)
		{
			m_FrameOfReference.setRotation( m_FrameOfReference.getRotation()*btQuaternion(dy,dx,dz) );
		}

		btVector3 GetLeft()
		{
			btMatrix3x3 basis = m_FrameOfReference.getBasis();
			return basis.getColumn(0);
		}
		btVector3 GetUp()
		{
			btMatrix3x3 basis = m_FrameOfReference.getBasis();
			return basis.getColumn(1);
		}
		btVector3 GetForward()
		{
			btMatrix3x3 basis = m_FrameOfReference.getBasis();
			return basis.getColumn(2);
		}
		btVector3 GetPosition()
		{
			return m_FrameOfReference.getOrigin();
		}
	};


	Camera m_Camera;

	btRigidBody* floorRigidBody;

	CVector m_TBNMatrix[2][3];
	CVector m_FloorVertexs[2][3];
	CVector2 m_FloorCoords[2][3];

	btAlignedObjectArray<Box*> m_apBoxes;

	//ID del programa de GLSL
	GLhandleARB m_hBumpProgram;

	// Posicion de la luz
	CVector m_vLightPos;

	// Las dos matrices TBN, una para cada triangulo de la escena. Cada matriz consiste de 3 vectores, ubicados como columnas.
	CVector g_TBNMatrix[2][3];

	DemoApp(): m_pDWorld(NULL), m_pDSolver(NULL), m_pCollisionConfiguration(NULL), m_pDispatcher(NULL), m_pBroadphase(NULL),
		m_WindowH(-1), m_pHand(NULL), m_hBumpProgram(NULL), m_vLightPos(0.0f, 0.0f, 33.0f),
		//m_Camera( btVector3(0,15,0), btVector3(0,0,1), btVector3(0,0,0), 45.0f*M_PI/180.0f, (GLfloat)800 / (GLfloat)640, 1.0f )
		//m_Camera( btVector3(-35,-35,15), btVector3(0,0,1), btVector3(0,0,0), 45.0f*M_PI/180.0f, (GLfloat)800 / (GLfloat)640, 1.0f )
		m_Camera( btVector3(-15,-15,15), btVector3(0,0,1), btVector3(0,0,5), 45.0f*M_PI/180.0f, (GLfloat)800 / (GLfloat)640, 1.0f )
	{}

	void CreateScene();

public:

	~DemoApp();

	std::map<std::string, Texture2D*> m_TextureMap;

	static DemoApp* GetInstance();

	bool isFrameThreshold(float dt);

	void Update();

	//Callbacks
	static void Render();

	static void keyPressed(unsigned char key, int x, int y);

	static void ReSizeGLScene(int Width, int Height);

	//Init App
	void Initialize(int windowH, int width, int height, int dFPS);

	float GetWidth();

	float GetHeight();

	void ReInitializeScene();

	void CrearShaderProgramaGLSL();
};

#endif

