#include "DemoApp.h"
#include <iostream>

//Box static members
CVector DemoApp::Box::m_vQuad[12][3];
CVector2 DemoApp::Box::m_vTexCoords[12][3];

//-----------------------------------------------------------------
//--------------	Funciones Auxiliares	-----------------------
//-----------------------------------------------------------------

//Calcula la matriz TBN que nos lleva desde el espacio Modelo al espacio de Textura(o Tangente)
void CalculateTBNMatrix(const CVector *pvTriangle, const CVector2 *pvTexCoords, CVector *pvTBNMatrix)
{
	// Calculamos la base del espacio tangente
	int i = 0;

	// Los indices de cercanos
	int nNextIndex = (i + 1) % 3;
	int nPrevIndex = (i + 2) % 3;

	// vJvI significa vector desde vertice I hasta J
	CVector v2v1 = pvTriangle[nNextIndex] - pvTriangle[i];
	CVector v3v1 = pvTriangle[nPrevIndex] - pvTriangle[i];

	// Usamos los valores intermedios:
	// c2c1_T = V2.texcoord.x – V1.texcoord.x
	// c2c1_B = V2.texcoord.y – V1.texcoord.y
	// c3c1_T = V3.texcoord.x – V1.texcoord.x
	// c3c1_B = V3.texcoord.y – V1.texcoord.y

	// Calculamos c2c1_T y c2c1_B
	float c2c1_T = pvTexCoords[nNextIndex].x - pvTexCoords[i].x;
	float c2c1_B = pvTexCoords[nNextIndex].y - pvTexCoords[i].y;

	// Calculamos c3c1_T y c3c1_B
	float c3c1_T = pvTexCoords[nPrevIndex].x - pvTexCoords[i].x;
	float c3c1_B = pvTexCoords[nPrevIndex].y - pvTexCoords[i].y;

	float fDenominator = c2c1_T * c3c1_B - c3c1_T * c2c1_B;
	if (ROUNDOFF(fDenominator) == 0.0f)	
	{
		// No nos arriesgamos a una division por 0, asi que ponemos la matriz como al identidad
		pvTBNMatrix[0] = CVector(1.0f, 0.0f, 0.0f);
		pvTBNMatrix[1] = CVector(0.0f, 1.0f, 0.0f);
		pvTBNMatrix[2] = CVector(0.0f, 0.0f, 1.0f);
	}
	else
	{
		float fScale1 = 1.0f / fDenominator;

		// T y B
		CVector T, B, N;
		T = CVector((c3c1_B * v2v1.x - c2c1_B * v3v1.x) * fScale1,
					(c3c1_B * v2v1.y - c2c1_B * v3v1.y) * fScale1,
					(c3c1_B * v2v1.z - c2c1_B * v3v1.z) * fScale1);

		B = CVector((-c3c1_T * v2v1.x + c2c1_T * v3v1.x) * fScale1,
					(-c3c1_T * v2v1.y + c2c1_T * v3v1.y) * fScale1,
					(-c3c1_T * v2v1.z + c2c1_T * v3v1.z) * fScale1);

		// La normal N la calculamos con el producto cruz
		N = T.CrossProduct(B);

		float fScale2 = 1.0f / ((T.x * B.y * N.z - T.z * B.y * N.x) + (B.x * N.y * T.z - B.z * N.y * T.x) + (N.x * T.y * B.z - N.z * T.y * B.x));
		
		// Calculamos la inversa de TBN
		pvTBNMatrix[0].x = B.CrossProduct(N).x * fScale2;
		pvTBNMatrix[0].y = -N.CrossProduct(T).x * fScale2;
		pvTBNMatrix[0].z = T.CrossProduct(B).x * fScale2;
		pvTBNMatrix[0].Normalize();

		pvTBNMatrix[1].x = -B.CrossProduct(N).y * fScale2;
		pvTBNMatrix[1].y = N.CrossProduct(T).y * fScale2;
		pvTBNMatrix[1].z = -T.CrossProduct(B).y * fScale2;
		pvTBNMatrix[1].Normalize();

		pvTBNMatrix[2].x = B.CrossProduct(N).z * fScale2;
		pvTBNMatrix[2].y = -N.CrossProduct(T).z * fScale2;
		pvTBNMatrix[2].z = T.CrossProduct(B).z * fScale2;
		pvTBNMatrix[2].Normalize();
		//Notemos que la matriz la almacenamos como vectores columna
	}
}

///////////////////////////////////////////////////////////////////
///////////////////////    Box     ////////////////////////////////
///////////////////////////////////////////////////////////////////

void DemoApp::Box::Initialize()
{
	// Bottom Face

	m_vQuad[0][0] = CVector(-1.0f, -1.0f, -1.0f)/*Top Right*/; m_vQuad[0][1] = CVector( 1.0f, -1.0f,  1.0f)/*Bottom Left*/; m_vQuad[0][2] = CVector(-1.0f, -1.0f,  1.0f)/*Bottom Right*/;
	m_vQuad[1][0] = CVector(-1.0f, -1.0f, -1.0f)/*Top Right*/; m_vQuad[1][1] = CVector( 1.0f, -1.0f, -1.0f)/*Top Left*/; m_vQuad[1][2] = CVector( 1.0f, -1.0f,  1.0f)/*Bottom Left*/;
	m_vTexCoords[0][0] = CVector2(1.0f, 1.0f)/*Top Right*/; m_vTexCoords[0][1] = CVector2(0.0f, 0.0f)/*Bottom Left*/; m_vTexCoords[0][2] = CVector2(1.0f, 0.0f)/*Bottom Right*/;
	m_vTexCoords[1][0] = CVector2(1.0f, 1.0f)/*Top Right*/; m_vTexCoords[1][1] = CVector2(0.0f, 1.0f)/*Top Left*/; m_vTexCoords[1][2] = CVector2(0.0f, 0.0f)/*Bottom Left*/;

    //glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  //  Of The Texture and Quad

    //glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  // Top Left Of The Texture and Quad

    //glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad

    //glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad

    // Front Face

	m_vQuad[2][0] = CVector( 1.0f,  1.0f,  1.0f)/*Top Right*/; m_vQuad[2][1] = CVector(-1.0f, -1.0f,  1.0f)/*Bottom Left*/; m_vQuad[2][2] = CVector( 1.0f, -1.0f,  1.0f)/*Bottom Right*/;
	m_vQuad[3][0] = CVector( 1.0f,  1.0f,  1.0f)/*Top Right*/; m_vQuad[3][1] = CVector(-1.0f,  1.0f,  1.0f)/*Top Left*/; m_vQuad[3][2] = CVector(-1.0f, -1.0f,  1.0f)/*Bottom Left*/;
	m_vTexCoords[2][0] = CVector2(1.0f, 1.0f)/*Top Right*/; m_vTexCoords[2][1] = CVector2(0.0f, 0.0f)/*Bottom Left*/; m_vTexCoords[2][2] = CVector2(1.0f, 0.0f)/*Bottom Right*/;
	m_vTexCoords[3][0] = CVector2(1.0f, 1.0f)/*Top Right*/; m_vTexCoords[3][1] = CVector2(0.0f, 1.0f)/*Top Left*/; m_vTexCoords[3][2] = CVector2(0.0f, 0.0f)/*Bottom Left*/;

    //glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad

    //glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad

    //glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // Top Right Of The Texture and Quad

    //glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // Top Left Of The Texture and Quad

    // Back Face

	m_vQuad[4][0] = CVector(-1.0f,  1.0f, -1.0f)/*Top Right*/; m_vQuad[4][1] = CVector( 1.0f, -1.0f, -1.0f)/*Bottom Left*/; m_vQuad[4][2] = CVector(-1.0f, -1.0f, -1.0f)/*Bottom Right*/;
	m_vQuad[5][0] = CVector(-1.0f,  1.0f, -1.0f)/*Top Right*/; m_vQuad[5][1] = CVector( 1.0f,  1.0f, -1.0f)/*Top Left*/; m_vQuad[5][2] = CVector( 1.0f, -1.0f, -1.0f)/*Bottom Left*/;
	m_vTexCoords[4][0] = CVector2(1.0f, 1.0f)/*Top Right*/; m_vTexCoords[4][1] = CVector2(0.0f, 0.0f)/*Bottom Left*/; m_vTexCoords[4][2] = CVector2(1.0f, 0.0f)/*Bottom Right*/;
	m_vTexCoords[5][0] = CVector2(1.0f, 1.0f)/*Top Right*/; m_vTexCoords[5][1] = CVector2(0.0f, 1.0f)/*Top Left*/; m_vTexCoords[5][2] = CVector2(0.0f, 0.0f)/*Bottom Left*/;

    //glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad

    //glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad

    //glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad

    //glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad

    // Right face

	m_vQuad[6][0] = CVector( 1.0f,  1.0f, -1.0f)/*Top Right*/; m_vQuad[6][1] = CVector( 1.0f, -1.0f,  1.0f)/*Bottom Left*/; m_vQuad[6][2] = CVector( 1.0f, -1.0f, -1.0f)/*Bottom Right*/;
	m_vQuad[7][0] = CVector( 1.0f,  1.0f, -1.0f)/*Top Right*/; m_vQuad[7][1] = CVector( 1.0f,  1.0f,  1.0f)/*Top Left*/; m_vQuad[7][2] = CVector( 1.0f, -1.0f,  1.0f)/*Bottom Left*/;
	m_vTexCoords[6][0] = CVector2(1.0f, 1.0f)/*Top Right*/; m_vTexCoords[6][1] = CVector2(0.0f, 0.0f)/*Bottom Left*/; m_vTexCoords[6][2] = CVector2(1.0f, 0.0f)/*Bottom Right*/;
	m_vTexCoords[7][0] = CVector2(1.0f, 1.0f)/*Top Right*/; m_vTexCoords[7][1] = CVector2(0.0f, 1.0f)/*Top Left*/; m_vTexCoords[7][2] = CVector2(0.0f, 0.0f)/*Bottom Left*/;

    //glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad

    //glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad

    //glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // Top Left Of The Texture and Quad

    //glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad

    // Left Face

	m_vQuad[8][0] = CVector(-1.0f,  1.0f,  1.0f)/*Top Right*/; m_vQuad[8][1] = CVector(-1.0f, -1.0f, -1.0f)/*Bottom Left*/; m_vQuad[8][2] = CVector(-1.0f, -1.0f,  1.0f)/*Bottom Right*/;
	m_vQuad[9][0] = CVector(-1.0f,  1.0f,  1.0f)/*Top Right*/; m_vQuad[9][1] = CVector(-1.0f,  1.0f, -1.0f)/*Top Left*/; m_vQuad[9][2] = CVector(-1.0f, -1.0f, -1.0f)/*Bottom Left*/;
	m_vTexCoords[8][0] = CVector2(1.0f, 1.0f)/*Top Right*/; m_vTexCoords[8][1] = CVector2(0.0f, 0.0f)/*Bottom Left*/; m_vTexCoords[8][2] = CVector2(1.0f, 0.0f)/*Bottom Right*/;
	m_vTexCoords[9][0] = CVector2(1.0f, 1.0f)/*Top Right*/; m_vTexCoords[9][1] = CVector2(0.0f, 1.0f)/*Top Left*/; m_vTexCoords[9][2] = CVector2(0.0f, 0.0f)/*Bottom Left*/;

    //glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad

    //glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad

    //glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // Top Right Of The Texture and Quad

    //glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad

	 // Top Face

	m_vQuad[10][0] = CVector( 1.0f,  1.0f, -1.0f)/*Top Right*/; m_vQuad[10][1] = CVector(-1.0f,  1.0f,  1.0f)/*Bottom Left*/; m_vQuad[10][2] = CVector( 1.0f,  1.0f,  1.0f)/*Bottom Right*/;
	m_vQuad[11][0] = CVector( 1.0f,  1.0f, -1.0f)/*Top Right*/; m_vQuad[11][1] = CVector(-1.0f,  1.0f, -1.0f)/*Top Left*/; m_vQuad[11][2] = CVector(-1.0f,  1.0f,  1.0f)/*Bottom Left*/;
	m_vTexCoords[10][0] = CVector2(1.0f, 1.0f)/*Top Right*/; m_vTexCoords[10][1] = CVector2(0.0f, 0.0f)/*Bottom Left*/; m_vTexCoords[10][2] = CVector2(1.0f, 0.0f)/*Bottom Right*/;
	m_vTexCoords[11][0] = CVector2(1.0f, 1.0f)/*Top Right*/; m_vTexCoords[11][1] = CVector2(0.0f, 1.0f)/*Top Left*/; m_vTexCoords[11][2] = CVector2(0.0f, 0.0f)/*Bottom Left*/;

    //glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad

    //glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // Bottom Left Of The Texture and Quad

    //glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // Bottom Right Of The Texture and Quad

    //glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
}

DemoApp::Box::Box(btDynamicsWorld* pDWorld, btRigidBody* pBox): m_DWorld(*pDWorld)
{
	//physic contruction
	m_pBox = pBox;
	m_DWorld.addRigidBody(m_pBox);

	//render contruction
	//cara inferior
	//m_vQuad[0] = { CVector(-.5, -.5f, -.5f), CVector(.5f, -.5f, -.5f), CVector(.5f, .5f, -.5f) };
	//m_vQuad[1] = { CVector(-.5f, .5f, -.5f), CVector(-.5f, -.5f, -.5f), CVector(.5f, -.5f, -.5f) };

	////cara superior
	//m_vQuad[2] = { CVector(.5f, -.5f, .5f), CVector(.5f, .5f, .5f), CVector(-.5, -.5f, .5f) };
	//m_vQuad[3] = { CVector(-.5f, -.5f, .5f), CVector(.5f, -.5f, .5f), CVector(-.5f, .5f, .5f) };

	////cara frente
	//m_vQuad[4] = { CVector(-.5f, .5f, -.5f), CVector(.5f, .5f, -.5f), CVector(.5, .5f, .5f) };
	//m_vQuad[5] = { CVector(-.5f, .5f, .5f), CVector(.5f, .5f, -.5f), CVector(-.5f, .5f, -.5f) };

	////cara atras
	//m_vQuad[6] = { CVector(.5f, -.5f, -.5f), CVector(.5, -.5f, .5f), CVector(-.5f, -.5f, -.5f) };
	//m_vQuad[7] = { CVector(.5f, -.5f, -.5f), CVector(-.5f, -.5f, -.5f), CVector(-.5f, -.5f, .5f) };

	//...
}

DemoApp::Box::~Box()
{
	//physic destroy
	m_DWorld.removeRigidBody(m_pBox);
	if( m_pBox->getMotionState() )
		delete m_pBox->getMotionState();
	delete m_pBox;
}

void DemoApp::Box::Update()
{
	m_pBox->getMotionState()->getWorldTransform(m_Transform);
	
	// Calculamoslas matrices TBN, para los 2 triangulos de la escena
	//Estas matrices solo sera necesario recalcularlas si se rotan los triangulos que forman la pared
	for(size_t k=0; k < 12 ; ++k)
	{
		static CVector transfVec[3];
		for(size_t j=0; j < 3 ; ++j)
		{
			btVector3 aux = m_Transform*btVector3(Box::m_vQuad[k][j].x, Box::m_vQuad[k][j].y, Box::m_vQuad[k][j].z);
			transfVec[j].x = aux.x();
			transfVec[j].y = aux.y();
			transfVec[j].z = aux.z();
		}
		CalculateTBNMatrix(transfVec, Box::m_vTexCoords[k], /*output*/m_TBNMatrix[k]);
	}
}

void DemoApp::Box::Render(GLhandleARB hGLSLProgram, const CVector &vLightPos)
{
	//Ponemos el programa de GLSL de bump mapping
	glUseProgramObjectARB(hGLSLProgram);

	//Cargamos la posicion de la luz
	GLint vLightPositionDir = glGetUniformLocationARB(hGLSLProgram, "vLightPosition");
	glUniform3fARB(vLightPositionDir, vLightPos.x, vLightPos.y, vLightPos.z);

	//Cargamos el color de la luz
	GLint vLightDiffuseColorDir = glGetUniformLocationARB(hGLSLProgram, "vLightDiffuseColor");
	glUniform3fARB(vLightDiffuseColorDir, 1.0f, 1.0f, 1.0f);

	//Cargamos el color de la luz ambiente
	GLint vAmbientLightColor = glGetUniformLocationARB(hGLSLProgram, "vAmbientLightColor");
	glUniform3fARB(vAmbientLightColor, 0.1f, 0.1f, 0.1f);

	GLint baseImageLoc = glGetUniformLocation(hGLSLProgram, "baseTexture");
	GLint normalMapLoc = glGetUniformLocation(hGLSLProgram, "normalTexture");
	glUniform1iARB(baseImageLoc, 0); //La unidad de textura 0 es la textura de color
	glUniform1iARB(normalMapLoc, 1); //La unidad de textura 1 es la textura de normales

	DemoApp &myApp = *DemoApp::GetInstance();

	// Establece la textura de roca
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, myApp.m_TextureMap["Roca"]->GetIDTextura());

	// Establece la textura de normales
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, myApp.m_TextureMap["RocaNormales"]->GetIDTextura());


	static btScalar matrix[16];

	glPushMatrix();
	m_Transform.getOpenGLMatrix(matrix);
	glMultMatrixf(matrix);

	glBegin(GL_TRIANGLES);
		for(size_t k=0; k < 12 ; ++k)
		{
			//Cargamos la matriz TBN para el triangulo k
			GLint vTangentDir = glGetAttribLocationARB(hGLSLProgram, "vTangent");
			glVertexAttrib3fARB(vTangentDir, m_TBNMatrix[k][0].x, m_TBNMatrix[k][0].y, m_TBNMatrix[k][0].z);

			GLint vBinormalDir = glGetAttribLocationARB(hGLSLProgram, "vBinormal");
			glVertexAttrib3fARB(vBinormalDir, m_TBNMatrix[k][1].x, m_TBNMatrix[k][1].y, m_TBNMatrix[k][1].z);

			GLint vNormalDir = glGetAttribLocationARB(hGLSLProgram, "vNormal");
			glVertexAttrib3fARB(vNormalDir, m_TBNMatrix[k][2].x, m_TBNMatrix[k][2].y, m_TBNMatrix[k][2].z);

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, Box::m_vTexCoords[k][0].x, Box::m_vTexCoords[k][0].y);
			glVertex3f(Box::m_vQuad[k][0].x, Box::m_vQuad[k][0].y, Box::m_vQuad[k][0].z);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, Box::m_vTexCoords[k][1].x, Box::m_vTexCoords[k][1].y);
			glVertex3f(Box::m_vQuad[k][1].x, Box::m_vQuad[k][1].y, Box::m_vQuad[k][1].z);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, Box::m_vTexCoords[k][2].x, Box::m_vTexCoords[k][2].y);
			glVertex3f(Box::m_vQuad[k][2].x, Box::m_vQuad[k][2].y, Box::m_vQuad[k][2].z);
		}
	glEnd();

	glPopMatrix();
	

	// Disable textures
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
}

///////////////////////////////////////////////////////////////////
///////////////////////    Hand    ////////////////////////////////
///////////////////////////////////////////////////////////////////

DemoApp::Hand::Hand(btDynamicsWorld* pDWorld, btRigidBody* pFingerPoints[6], float radioEsferas, float handMinDepth, float handReach):
	m_DWorld(*pDWorld), m_radioEsferas(radioEsferas), m_handMinDepth(handMinDepth), m_handReach(handReach)
{
	//physic contruction
	for(size_t k=0; k < 5 ;++k)
	{
		m_pFingerPoints[k] = pFingerPoints[k];
		m_DWorld.addRigidBody(m_pFingerPoints[k]);
	}
	m_pFingerPoints[5] = pFingerPoints[5];// could be NULL

	//render contruction
	m_pFingerQuadric = gluNewQuadric();

	gluQuadricOrientation(m_pFingerQuadric, GLU_OUTSIDE);//normales hacia afuera

	gluQuadricDrawStyle(m_pFingerQuadric, GLU_FILL);//triangulos llenos

	gluQuadricNormals(m_pFingerQuadric, GLU_SMOOTH);//normales per vertex

	gluQuadricTexture(m_pFingerQuadric, GL_TRUE);//con coordenadas de texturas

	gluQuadricCallback(m_pFingerQuadric, GLU_ERROR, NULL);//por ahora ningun call back de error

	DemoApp &myApp = *DemoApp::GetInstance();
	m_GLDList = glGenLists(1);
	glNewList(m_GLDList, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, myApp.m_TextureMap["Azul"]->GetIDTextura());
		gluSphere(m_pFingerQuadric, m_radioEsferas, 30, 30);//una esfera
	glEndList();

	m_PrevDepth.push_back(-999999);
	m_PrevDepth.push_back(-999999);
	m_PrevDepth.push_back(-999999);
}

DemoApp::Hand::~Hand()
{
	//render destroy
	gluDeleteQuadric(m_pFingerQuadric);

	//physic destroy
	for(size_t k=0; k < 5 ;++k)
	{
		m_DWorld.removeRigidBody(m_pFingerPoints[k]);
		if( m_pFingerPoints[k]->getMotionState() )
			delete m_pFingerPoints[k]->getMotionState();
		delete m_pFingerPoints[k];
	}

	if( m_pFingerPoints[5] != NULL )
	{
		m_DWorld.removeRigidBody(m_pFingerPoints[5]);
		if( m_pFingerPoints[5]->getMotionState() )
			delete m_pFingerPoints[5]->getMotionState();
		delete m_pFingerPoints[5];
	}
}

void DemoApp::Hand::GetFrustumAreaAtDepth(float zDepth, float &widthZDepth, float &heightZDepth)
{
	DemoApp& app = *DemoApp::GetInstance();

	float fov = app.m_Camera.m_fov;
	float ratio =  app.m_Camera.m_ratio;

	heightZDepth = 2*(sin(fov)/cos(fov)*zDepth);
	widthZDepth  = ratio*heightZDepth;
}

void DemoApp::Hand::Update()
{
	
	Esqueleto& mano = *Esqueleto::GetInstance();
	DemoApp& app = *DemoApp::GetInstance();

	float widthZDepth=0, heightZDepth=0;
	float ZDepth = app.m_Camera.m_ZNear+m_handMinDepth;
	GetFrustumAreaAtDepth( ZDepth, widthZDepth, heightZDepth);

	int maxIt = (m_pFingerPoints[5] != NULL)? mano.GetNumeroDedos()+1 : mano.GetNumeroDedos();
	for(size_t k=0; k < maxIt ;++k)
	{
		float xDedo=0, yDedo=0, zDedo=0;
		mano.GetPosicionPuntaDedo(k, xDedo, yDedo, zDedo);
		if( xDedo != xDedo || yDedo != yDedo || zDedo != zDedo )//the finger doesnt exist
			continue;
		btTransform a;
		a.setIdentity();

		yDedo = (1-yDedo);
		xDedo -= 0.5;
		yDedo -= 0.5;

		///PROMEDIO TEMPORAL
		m_PrevDepth.erase(m_PrevDepth.begin());
		m_PrevDepth.push_back(zDedo);
		float prom = 0;
		for(int f=0; f < m_PrevDepth.size() ;++f)
		{
			prom += m_PrevDepth[f];
		}
		prom /= m_PrevDepth.size();
		zDedo = prom;
		///
		//std::cerr<< "zDedo: "<<zDedo<<std::endl;
		
		btVector3 cx = -app.m_Camera.GetLeft()*xDedo*widthZDepth;
		btVector3 cy = app.m_Camera.GetUp()*yDedo*heightZDepth;
		btVector3 cz = app.m_Camera.GetForward()*(ZDepth+zDedo*m_handReach);

		a.setOrigin(app.m_Camera.GetPosition() + cx + cy + cz);
		m_pFingerPoints[k]->getMotionState()->setWorldTransform( a );
	}
	
	for(size_t k=0; k < maxIt ;++k)
		m_pFingerPoints[k]->getMotionState()->getWorldTransform(m_Transforms[k]);
	#ifdef CONSOLE_VERBOSE
		std::cerr<<"("<<m_Transforms[0].getOrigin().x()<<","<<
				m_Transforms[0].getOrigin().y()<<","<<
				m_Transforms[0].getOrigin().z()<<")"<<std::endl;
	#endif
}

void DemoApp::Hand::Render()
{
	static btScalar matrix[16];

	DemoApp &myApp = *DemoApp::GetInstance();
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, myApp.m_TextureMap["Azul"]->GetIDTextura());
	for(size_t k=0; k < 5 ;++k)
	{
		glPushMatrix();
		m_Transforms[k].getOpenGLMatrix(matrix);
		glMultMatrixf(matrix);

		gluSphere(m_pFingerQuadric, m_radioEsferas, 30, 30);//una esfera
		//glCallList(m_GLDList);
		glPopMatrix();
	}
	if( m_pFingerPoints[5] != NULL )
	{
		glPushMatrix();
		m_Transforms[5].getOpenGLMatrix(matrix);
		glMultMatrixf(matrix);

		gluSphere(m_pFingerQuadric, m_radioEsferas*2, 30, 30);//una esfera
		//glCallList(m_GLDList);
		glPopMatrix();
	}
}

///////////////////////////////////////////////////////////////////
////////////////////    DemoApp    ////////////////////////////////
///////////////////////////////////////////////////////////////////

DemoApp::~DemoApp()
{
	//hand destroy
	delete m_pHand;

	//floor destroy
	m_pDWorld->removeRigidBody(floorRigidBody);
	delete floorRigidBody->getMotionState();
	delete floorRigidBody;
 
	//shapes destroy
    m_apShapes.clear();

	//bullet physic destroy
	if( m_pDWorld )
		delete m_pDWorld;
	if( m_pDSolver )
        delete m_pDSolver;
	if( m_pCollisionConfiguration )
        delete m_pCollisionConfiguration;
	if( m_pDispatcher )
        delete m_pDispatcher;
	if( m_pBroadphase )
        delete m_pBroadphase;

	//Borramos el programa de GLSL y sus shaders
	glDeleteObjectARB(m_hBumpProgram);
}

DemoApp* DemoApp::GetInstance()
{
	static DemoApp app;
	return &app;
}

void DemoApp::CreateScene()
{
		m_pBroadphase = new btDbvtBroadphase();
 
        m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
        m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);
 
        m_pDSolver = new btSequentialImpulseConstraintSolver;
 
        m_pDWorld = new btDiscreteDynamicsWorld(m_pDispatcher,m_pBroadphase,m_pDSolver,m_pCollisionConfiguration);
 
        m_pDWorld->setGravity(btVector3(0,0,-1.0));


		//Plano del piso
		{
			btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,0,1),1);
			m_apShapes.push_back(groundShape);

			btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
			btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
			floorRigidBody = new btRigidBody(groundRigidBodyCI);
			m_pDWorld->addRigidBody(floorRigidBody);
		}
 
		//Esfera
		{
			float radioEsferas = 0.1f;
			btCollisionShape* fallShape = new btSphereShape(radioEsferas);
			m_apShapes.push_back(fallShape);
			btScalar mass = 0;//kinematic body
			btVector3 fallInertia(0,0,0);
			fallShape->calculateLocalInertia(mass,fallInertia);
			btRigidBody* esferas[5+1];//5 dedos mas centro de la mano
			for(size_t k=0; k < (5+1) ;++k)
			{
				int x=rand()%10-5;
				int y=rand()%10-5;
				btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(x,y,20)));
				btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallShape,fallInertia);
				esferas[k] = new btRigidBody(fallRigidBodyCI);
				esferas[k]->setCollisionFlags(esferas[k]->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
				esferas[k]->setActivationState(DISABLE_DEACTIVATION);

			}

			m_pHand = new Hand(m_pDWorld, esferas, radioEsferas, 5, 65);
		}

		//Cajas
		{
			btCollisionShape* boxShape = new btBoxShape(btVector3(1.0f,1.0f,1.0f));
			m_apShapes.push_back(boxShape);
			btScalar mass = 1;
			btVector3 boxInertia(0,0,0);
			boxShape->calculateLocalInertia(mass,boxInertia);
			for(size_t h=0; h < 3 ;++h)
				for(size_t k=0; k < 5 ;++k)
				{
					int x=h*2.5-2.5;
					int z=k*2.5f+5;
					btDefaultMotionState* boxMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(x,0,z)));
					btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass,boxMotionState,boxShape,boxInertia);
					btRigidBody* pBox = new btRigidBody(boxRigidBodyCI);
					m_apBoxes.push_back(new Box(m_pDWorld, pBox) );
					pBox->setActivationState(DISABLE_DEACTIVATION);
				}
		}
		
}

bool DemoApp::isFrameThreshold(float dt)
{
	return (dt > m_milisecondsDelta);
}

void DemoApp::Update()
{
    m_pDWorld->stepSimulation(7*1/60.f,10);

	//update hand
	if( current_state == ESQUELETO_DETECTADO || current_state == DEDOS_DETECTADOS || current_state == APLICACION_CORRIENDO )// just if in run state
		m_pHand->Update();

	//update boxes
	for(size_t k=0; k < m_apBoxes.size() ;++k)
		m_apBoxes[k]->Update();
}

void DemoApp::Render()
{
	DemoApp &myApp = *DemoApp::GetInstance();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// rellena con color de fondo

	//Sacamos el programa de GLSL
	glUseProgramObjectARB(NULL);

	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//glFrustum(-20, 20, -20, 20, 0.1, 1000);
		gluPerspective(45.0f, (GLfloat)myApp.GetWidth() / (GLfloat)myApp.GetHeight(), 2.0f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		btVector3 Position = myApp.m_Camera.GetPosition();
		btVector3 Up = myApp.m_Camera.GetUp();
		btVector3 lookAt = myApp.m_Camera.GetPosition() + myApp.m_Camera.GetForward();
		//gluLookAt(myApp.m_Camera.x, myApp.m_Camera.y, myApp.m_Camera.z, 
		gluLookAt(myApp.m_Camera.GetPosition().x(), myApp.m_Camera.GetPosition().y(), myApp.m_Camera.GetPosition().z(), 
				  lookAt.x(), lookAt.y(), lookAt.z(),
				  myApp.m_Camera.GetUp().x(), myApp.m_Camera.GetUp().y(), myApp.m_Camera.GetUp().z() );
	
	
	
	/////

	//Ponemos el programa de GLSL de bump mapping
	glUseProgramObjectARB(myApp.m_hBumpProgram);

	//Cargamos la posicion de la luz
	GLint vLightPositionDir = glGetUniformLocationARB(myApp.m_hBumpProgram, "vLightPosition");
	glUniform3fARB(vLightPositionDir, myApp.m_vLightPos.x, myApp.m_vLightPos.y, -myApp.m_vLightPos.z);

	//Cargamos el color de la luz
	GLint vLightDiffuseColorDir = glGetUniformLocationARB(myApp.m_hBumpProgram, "vLightDiffuseColor");
	glUniform3fARB(vLightDiffuseColorDir, 1.0f, 1.0f, 1.0f);

	//Cargamos el color de la luz ambiente
	GLint vAmbientLightColor = glGetUniformLocationARB(myApp.m_hBumpProgram, "vAmbientLightColor");
	glUniform3fARB(vAmbientLightColor, 0.1f, 0.1f, 0.1f);

	GLint baseImageLoc = glGetUniformLocation(myApp.m_hBumpProgram, "baseTexture");
	GLint normalMapLoc = glGetUniformLocation(myApp.m_hBumpProgram, "normalTexture");
	glUniform1iARB(baseImageLoc, 0); //La unidad de textura 0 es la textura de color
	glUniform1iARB(normalMapLoc, 1); //La unidad de textura 1 es la textura de normales

	// Establece la textura de roca
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, myApp.m_TextureMap["GrassMud"]->GetIDTextura());

	// Establece la textura de normales
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, myApp.m_TextureMap["GrassMudNM"]->GetIDTextura());

	/////

	
	//glActiveTextureARB(GL_TEXTURE0_ARB);
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, myApp.m_TextureMap["Floor"]->GetIDTextura());
	glBegin(GL_TRIANGLES);
	for(size_t k=0; k < 2 ;++k)
	{
		//Cargamos la matriz TBN para el triangulo 1
		GLint vTangentDir = glGetAttribLocationARB(myApp.m_hBumpProgram, "vTangent");
		glVertexAttrib3fARB(vTangentDir, myApp.m_TBNMatrix[k][0].x, myApp.m_TBNMatrix[k][0].y, myApp.m_TBNMatrix[k][0].z);

		GLint vBinormalDir = glGetAttribLocationARB(myApp.m_hBumpProgram, "vBinormal");
		glVertexAttrib3fARB(vBinormalDir, myApp.m_TBNMatrix[k][1].x, myApp.m_TBNMatrix[k][1].y, myApp.m_TBNMatrix[k][1].z);

		GLint vNormalDir = glGetAttribLocationARB(myApp.m_hBumpProgram, "vNormal");
		glVertexAttrib3fARB(vNormalDir, myApp.m_TBNMatrix[k][2].x, myApp.m_TBNMatrix[k][2].y, myApp.m_TBNMatrix[k][2].z);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, myApp.m_FloorCoords[k][0].x, myApp.m_FloorCoords[k][0].y);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, myApp.m_FloorCoords[k][0].x, myApp.m_FloorCoords[k][0].y);
		glVertex3f(myApp.m_FloorVertexs[k][0].x, myApp.m_FloorVertexs[k][0].y, myApp.m_FloorVertexs[k][0].z);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, myApp.m_FloorCoords[k][1].x, myApp.m_FloorCoords[k][1].y);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, myApp.m_FloorCoords[k][1].x, myApp.m_FloorCoords[k][1].y);
		glVertex3f(myApp.m_FloorVertexs[k][1].x, myApp.m_FloorVertexs[k][1].y, myApp.m_FloorVertexs[k][1].z);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, myApp.m_FloorCoords[k][2].x, myApp.m_FloorCoords[k][2].y);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, myApp.m_FloorCoords[k][2].x, myApp.m_FloorCoords[k][2].y);
		glVertex3f(myApp.m_FloorVertexs[k][2].x, myApp.m_FloorVertexs[k][2].y, myApp.m_FloorVertexs[k][2].z);
	}
	/*
		//						V1----V2
		//						|     |
		//						V3----V4

		float h = 100;

			
		// El vertice inferior izquierdo
		glTexCoord2f(0.0f, 10.0f);
		glVertex3f(-h, -h, 0.0f);

		// ------------------------------------------------------------ //

		// El vertice inferior derecho
		glTexCoord2f(10.0f, 10.0f);
		glVertex3f(h, -h, 0.0f);

		// ------------------------------------------------------------ //

		// El vertice superior derecho
		glTexCoord2f(10.0f, 0.0f);
		glVertex3f(h, h, 0.0f);

		// ------------------------------------------------------------ //

			//Cargamos la matriz TBN para el triangulo 2
			GLint vTangentDir = glGetAttribLocationARB(myApp.m_hBumpProgram, "vTangent");
			glVertexAttrib3fARB(vTangentDir, myApp.m_TBNMatrix[1][0].x, myApp.m_TBNMatrix[1][0].y, myApp.m_TBNMatrix[1][0].z);

			GLint vBinormalDir = glGetAttribLocationARB(myApp.m_hBumpProgram, "vBinormal");
			glVertexAttrib3fARB(vBinormalDir, myApp.m_TBNMatrix[1][1].x, myApp.m_TBNMatrix[1][1].y, myApp.m_TBNMatrix[1][1].z);

			GLint vNormalDir = glGetAttribLocationARB(myApp.m_hBumpProgram, "vNormal");
			glVertexAttrib3fARB(vNormalDir, myApp.m_TBNMatrix[1][2].x, myApp.m_TBNMatrix[1][2].y, myApp.m_TBNMatrix[1][2].z);

		// El vertice superior derecho
		glTexCoord2f(10.0f, 0.0f);
		glVertex3f(h, h, 0.0f);

		// El vertice superior izquierdo
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-h, h, 0.0f);

		// El vertice inferior izquierdo
		glTexCoord2f(0.0f, 10.0f);
		glVertex3f(-h, -h, 0.0f);
		*/
	glEnd();

	//render hand
	if( current_state == ESQUELETO_DETECTADO || current_state == DEDOS_DETECTADOS || current_state == APLICACION_CORRIENDO )// just if in run state
		myApp.m_pHand->Render();

	//render boxes
	for(size_t k=0; k < myApp.m_apBoxes.size() ;++k)
		myApp.m_apBoxes[k]->Render(myApp.m_hBumpProgram, myApp.m_vLightPos);
	

	glDisable(GL_TEXTURE_2D);

	glutSwapBuffers();

	int errornum = glGetError();
	while( errornum != GL_NO_ERROR )
	{
		if(errornum==GL_INVALID_ENUM)
			std::cerr << "GL_INVALID_ENUM" << std::endl;
		else if( errornum == GL_INVALID_VALUE )
			std::cerr << "GL_INVALID_VALUE" << std::endl;
		else if( errornum == GL_INVALID_OPERATION )
			std::cerr << "GL_INVALID_OPERATION" << std::endl;
		else if( errornum == GL_STACK_OVERFLOW )
			std::cerr << "GL_STACK_OVERFLOW" << std::endl;
		else if( errornum == GL_STACK_UNDERFLOW )
			std::cerr << "GL_STACK_UNDERFLOW" << std::endl;
		else if( errornum == GL_OUT_OF_MEMORY )
			std::cerr << "GL_OUT_OF_MEMORY" << std::endl;

		errornum = glGetError();
	}
}

void DemoApp::keyPressed(unsigned char key, int x, int y)
{
	DemoApp &myApp = *DemoApp::GetInstance();
	if(key == 'w')
		myApp.m_Camera.Move(0,0,1);
	if(key == 's')
		myApp.m_Camera.Move(0,0,-1);
	if(key == 'a')
		myApp.m_Camera.Move(1,0,0);
	if(key == 'd')
		myApp.m_Camera.Move(-1,0,0);
	if(key == 'q')
		myApp.m_Camera.Rotate(0,0.05,0);
	if(key == 'e')
		myApp.m_Camera.Rotate(0,-0.05,0);
	if(key == 'r')
		myApp.m_Camera.Rotate(-0.05,0,0);
	if(key == 'f')
		myApp.m_Camera.Rotate(0.05,0,0);
	if(key == 'p')
		myApp.ReInitializeScene();
}

void DemoApp::ReSizeGLScene(int Width, int Height)
{
	DemoApp &myApp = *DemoApp::GetInstance();
	myApp.m_width = Width; myApp.m_height = Height;

	glViewport(0,0,Width,Height);

	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//glFrustum(-20, 20, -20, 20, 0.1, 1000);
		//glOrtho(0,Width,0,Height,-1,1);
		gluPerspective(45.0f, (GLfloat)Width / (GLfloat)Height, 1.0f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(myApp.m_Camera.x, myApp.m_Camera.y, myApp.m_Camera.z, /* look from camera XYZ */
				  0, 0, 0, /* look at the origin */
                  0, 0, 1); /* positive Y up vector */
		

	glutPostWindowRedisplay(myApp.m_WindowH);
}

void DemoApp::Initialize(int windowH, int width, int height, int dFPS)
{
	m_milisecondsDelta = ((double)1/dFPS)*1000;//dFPS son FPS deseados

	m_WindowH = windowH;

	glutReshapeFunc(&DemoApp::ReSizeGLScene);
	glutKeyboardFunc(&DemoApp::keyPressed);
	glutDisplayFunc(&DemoApp::Render);

	// OpenGL config
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	m_width = width;
	m_height = height;
	DemoApp::ReSizeGLScene(m_width, m_height);

	m_Camera.x = -35; m_Camera.y = -35; m_Camera.z = 15;
	

	Box::Initialize();//initialize vertexs and texcoords

	//Comprobamos las capacidades de OpenGL
	glewInit();

	//Verificamos que soporte GLSL como una extension
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		std::cout<<"Soporta GLSL"<<std::endl;
	else
	{
		std::cout<<"No soporta GLSL..."<<std::endl;
		system("pause");
		exit(1);
	}
	CrearShaderProgramaGLSL();

	m_TextureMap["Roca"] = new Texture2D("..\\rock.bmp");
	m_TextureMap["RocaNormales"] = new Texture2D("..\\rockNormal.bmp");
	m_TextureMap["Azul"] = new Texture2D("..\\azul.png");
	m_TextureMap["Floor"] = new Texture2D("..\\grass.ppm");
	m_TextureMap["Desert"] = new Texture2D("..\\DesertChic.png");
	m_TextureMap["DesertNM"] = new Texture2D("..\\DesertChic(Normal Map).png");
	m_TextureMap["GrassMud"] = new Texture2D("..\\GrassMud.png");
	m_TextureMap["GrassMudNM"] = new Texture2D("..\\GrassMud(Normal Map).png");

	//						V1----V2
	//						|     |
	//						V3----V4

	float h = 100;
	int reps = 40;

	// El vertice superior derecho
	m_FloorCoords[0][0] = CVector2(reps, 0.0f);
	m_FloorVertexs[0][0] = CVector(h, h, 0.0f);

	// El vertice inferior derecho
	m_FloorCoords[0][1] = CVector2(reps, reps);
	m_FloorVertexs[0][1] = CVector(h, -h, 0.0f);

	// El vertice inferior izquierdo
	m_FloorCoords[0][2] = CVector2(0.0f, reps);
	m_FloorVertexs[0][2] = CVector(-h, -h, 0.0f);

	// ------------------------------------------------------------ //

	// El vertice inferior izquierdo
	m_FloorCoords[1][0] = CVector2(0.0f, reps);
	m_FloorVertexs[1][0] = CVector(-h, -h, 0.0f);

	// El vertice superior izquierdo
	m_FloorCoords[1][1] = CVector2(0.0f, 0.0f);
	m_FloorVertexs[1][1] = CVector(-h, h, 0.0f);

	// El vertice superior derecho
	m_FloorCoords[1][2] = CVector2(reps, 0.0f);
	m_FloorVertexs[1][2] = CVector(h, h, 0.0f);


	// Calculamoslas matrices TBN, para los 2 triangulos del suelo
	for(size_t k=0; k < 2 ; ++k)
		CalculateTBNMatrix(m_FloorVertexs[k], m_FloorCoords[k], /*output*/m_TBNMatrix[k]);

	CreateScene();
}

float DemoApp::GetWidth()
{
	return m_width;
}

float DemoApp::GetHeight()
{
	return m_height;
}

void DemoApp::ReInitializeScene()
{
	//physic destroy
	for(int k=0; k < m_apBoxes.size() ;++k)
		delete m_apBoxes[k];
	m_apBoxes.clear();

	//creacion de cajas
	{
		btCollisionShape* boxShape = new btBoxShape(btVector3(1.0f,1.0f,1.0f));
		m_apShapes.push_back(boxShape);
		btScalar mass = 1;
		btVector3 boxInertia(0,0,0);
		boxShape->calculateLocalInertia(mass,boxInertia);
		for(size_t h=0; h < 3 ;++h)
			for(size_t k=0; k < 5 ;++k)
			{
				int x=h*2.5-2.5;
				int z=k*2.5f+5;
				btDefaultMotionState* boxMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(x,0,z)));
				btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass,boxMotionState,boxShape,boxInertia);
				btRigidBody* pBox = new btRigidBody(boxRigidBodyCI);
				m_apBoxes.push_back(new Box(m_pDWorld, pBox) );
				pBox->setActivationState(DISABLE_DEACTIVATION);
			}
	}
}

char* textFileRead(char *fn)
{
	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) {
		fp = fopen(fn,"rt");

		if (fp != NULL) {
      
      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      rewind(fp);

			if (count > 0) {
				content = new char[count+1];
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

//Esta funcion crea los shaders: vertex shader y fragment shader
//Y luego crea un programa de GLSL linkeado con esos dos shader
//En resumen tendra el programa de handle handleP con el programa de bump mapping
void DemoApp::CrearShaderProgramaGLSL()
{
	//Creamos el shader contenedor
	GLhandleARB handleVS = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	GLhandleARB handleFS = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	//Cargamos los codigos fuente de los shaders de GLSL
	char* vs = textFileRead("..\\VertexShader.vert");
	char* fs = textFileRead("..\\FragmentShader.frag");

	const char * vv = vs;
	const char * ff = fs;

	//Le asignamos el codigo fuente del shader
	glShaderSourceARB(handleVS, 1, &vv, NULL);
	glShaderSourceARB(handleFS, 1, &ff, NULL);

	delete[] vs;
	delete[] fs;

	//Compilamos el shader
	glCompileShaderARB(handleVS);
	glCompileShaderARB(handleFS);

	//Verificamos que compilaron bien
	int* resp= new int;
	glGetObjectParameterivARB(handleVS, GL_OBJECT_COMPILE_STATUS_ARB, resp);
	if( *resp == 0 )
	{
		std::cout<<"El vertex shader no pudo compilar!"<<std::endl;
		system("pause");
	}
	glGetObjectParameterivARB(handleFS, GL_OBJECT_COMPILE_STATUS_ARB, resp);
	if( *resp == 0 )
	{
		std::cout<<"El fragment shader no pudo compilar!"<<std::endl;
		system("pause");
	}

	//Creamos el programa contenedor de los shaders
	m_hBumpProgram = glCreateProgramObjectARB();

	//Agregamos los shaders al programa
	glAttachObjectARB(m_hBumpProgram, handleVS);
	glAttachObjectARB(m_hBumpProgram, handleFS);

	//Linkeamos el programa
	glLinkProgramARB(m_hBumpProgram);

	//Verificamos que linkeo bien
	glGetObjectParameterivARB(handleVS, GL_OBJECT_LINK_STATUS_ARB, resp);
	if( *resp == 0 )
	{
		std::cout<<"El vertex shader no pudo linkear!"<<std::endl;
		system("pause");
	}
	glGetObjectParameterivARB(handleFS, GL_OBJECT_LINK_STATUS_ARB, resp);
	if( *resp == 0 )
	{
		std::cout<<"El fragment shader no pudo linkear!"<<std::endl;
		system("pause");
	}

	delete resp;
}
