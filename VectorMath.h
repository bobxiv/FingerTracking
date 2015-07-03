#pragma once

#include <math.h>

// Definimos PI
#define PI 3.1415926535897932

// Si un valor es muy cercano a 0 lo hace 0
#define ROUNDOFF(x) (x < 0.0001f && x > -0.0001f) ? 0.0f : x

// Clase que representa un vector 3D y permite realizar varias operaciones con estos
// Los detalles de la implementacion de esta clase no son de importancia para el ejemplo
class CVector
{
public:
	//Componentes
	float x, y, z;

public:
	// Constructores
	CVector()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
	{
	}

	CVector(float X, float Y, float Z)
	: x(X)
	, y(Y)
	, z(Z)
	{
	}

	// Suma de 2 vectores
	// Permite realizar operaciones como C=A+B
	// Siendo A, B y C vectores
	CVector operator+(const CVector &vec) const
	{
		return CVector(x + vec.x, y + vec.y, z + vec.z);
	}

	// Resta de 2 vectores
	// Permite realizar operaciones como C=A-B
	// Siendo A, B y C vectores
	CVector operator-(const CVector &vec) const
	{
		return CVector(x - vec.x, y - vec.y, z - vec.z);
	}

	// Division vector por escalar
	// Permite realizar operaciones como C/=a
	// Siendo C un vector y a un escalar
	CVector operator/=(const float &fScalar)
	{
		// To achieve speed we only perform 1 division
		float fTemp = 1 / fScalar;
		x *= fTemp;
		y *= fTemp;
		z *= fTemp;

		return *this;
	}

	// Devuelve la longitud del vector
	// Usa distancia Euclidea
	float Length() const
	{
		return sqrtf(x*x + y*y + z*z);
	}

	// Normaliza el vector
	void Normalize()
	{
		(*this) /= Length();
	}

	// Realiza el producto interno entre vectores
	// Por ejemplo c=<A,B>
	// Siendo A y B vectores y <A,B> el producto interno entre ellos
	// c es un escalar
	float DotProduct(const CVector &vec) const
	{
		return (x * vec.x + y * vec.y + z * vec.z);
	}

	// Realiza el producto cruz entre vectores
	// Por ejemplo C=AxB
	// Siendo A y B vectores y AxB el producto cruz entre ellos
	// c es un vector
	CVector CrossProduct(const CVector &vec) const
	{
		return CVector((y * vec.z - z * vec.y), (z * vec.x - x * vec.z), (x * vec.y - y * vec.x));
	}
};

// Clase que representa un vector 2D
// A diferencia de CVector esta clase no permite realizar operaciones entre estos vectores
class CVector2
{
public:
	float x, y;

public:
	// Constructor
		CVector2()
	: x(0.0f)
	, y(0.0f)
	{	}

	CVector2(float X, float Y)
	: x(X)
	, y(Y)
	{	}
};