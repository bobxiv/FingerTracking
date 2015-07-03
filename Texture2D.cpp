#include "Texture2D.h"
#include <exception>

#include "BMPFile.h"
#include "TGAFile.h"
#include "PPMFile.h"
#include "PNGFile.h"

Texture2D::Texture2D(std::string nombre, int textureWarpS, int textureWarpT, int textureMagFilter, int textureMinFilter)
{
	ImageFile* pImagen = NULL;
	//Intenta cargar el archivo de imagen(dependiendo de la extension del archivo es la clase que usa)
	try
	{
		_CargarImagen(nombre, pImagen);
	}catch( std::exception& e )
	{
		throw e;
	}

	//Crear y configurar la textura de OpenGL
	glGenTextures(1, &m_IDTexturaOpenGL);
	glBindTexture(GL_TEXTURE_2D, m_IDTexturaOpenGL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , textureWarpS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , textureWarpT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureMinFilter);

	glPixelStorei ( GL_UNPACK_ALIGNMENT,   1 );// Alineados a 1 byte ya que las imagenes se cargan sin padding(ie alineadas a 1 byte)
	glPixelStorei ( GL_UNPACK_ROW_LENGTH,  0 );
	glPixelStorei ( GL_UNPACK_SKIP_ROWS,   0 );
	glPixelStorei ( GL_UNPACK_SKIP_PIXELS, 0 );

	switch( m_EspacioColores )
	{
	case ImageFile::R8_G8_B8:
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, pImagen->GetColores());
		break;
	case ImageFile::R8_G8_B8_A8:
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImagen->GetColores());
		break;
	}
	//Luego de esto ya se podria eliminar el arreglo m_ArregloColores, pero lo conservaremos y lo eliminaremos
	//en el destructor de la clase
	delete pImagen;
}

void Texture2D::_CargarImagen(std::string nombre, ImageFile* &pImagen)
{
	std::string::size_type pos = nombre.rfind('.');
	std::string extension = nombre.substr(pos+1);

	if( extension == "bmp" )
	{
		pImagen = new BMPFile;
	}else if( extension == "tga" )
	{
		pImagen = new TGAFile;
	}else if( extension == "ppm" )
	{
		pImagen = new PPMFile;
	}else if( extension == "png" )
	{
		pImagen = new PNGFile;
	}else if( extension == "jpg" )
	{
		//JPGFile(nombre);
	}

	pImagen->LoadFromFile(nombre);
	m_Width          = pImagen->GetWidth();
	m_Height         = pImagen->GetHeight();
	m_EspacioColores = pImagen->GetEspacioColores();
}

Texture2D::~Texture2D()
{
	glDeleteTextures( 1, &m_IDTexturaOpenGL );
}

ImageFile::EspacioColores Texture2D::GetEspacioColores()
{
	return m_EspacioColores;
}

int Texture2D::GetWidth()
{
	return m_Width;
}

int Texture2D::GetHeight()
{
	return m_Height;
}

GLuint Texture2D::GetIDTextura()
{
	return m_IDTexturaOpenGL;
}
