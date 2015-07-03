#pragma once
#include <string>

#include <GL/glut.h>

#include "ImageFile.h"

class Texture2D
{
private:
	
	ImageFile::EspacioColores m_EspacioColores;

	int                       m_Width;
	int                       m_Height;

	GLuint                    m_IDTexturaOpenGL;

	void _CargarImagen(std::string nombre, ImageFile* &pImagen);

public:

	Texture2D(std::string nombre, int textureWarpS = GL_REPEAT, int textureWarpT = GL_REPEAT, int textureMagFilter = GL_LINEAR, int textureMinFilter = GL_LINEAR);

	~Texture2D();

	ImageFile::EspacioColores GetEspacioColores();

	int GetWidth();

	int GetHeight();

	GLuint GetIDTextura();
};