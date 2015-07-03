//Versores del Espacio de Texturas(o tangente)
attribute vec3 vTangent;
attribute vec3 vBinormal;
attribute vec3 vNormal;

//Posicion de la luz
uniform vec3 vLightPosition;

//Vector desde vertice ahora y luego desde fragmento hasta la luz
varying vec3 vLightVector;

//Algunas variables predefinidas que podemos llegar a utilizar:
// uniform mat4 gl_ProjectionMatrix -> Matriz de Proyecion sacada del estado interno de OpenGL
// uniform mat4 gl_ModelViewMatrix  -> Matriz de ModelView sacada del estado interno de OpenGL
// attribute vec4 gl_Vertex         -> Vector de entrada de la posicion del vertice del VS
// attribute vec4 gl_Position       -> Vector de salida de la posicion del vertice del VS
// attribute vec4 gl_MultiTexCoord0 -> Coordenada de textura (u,v) de textura numero 0
// attribute vec4 gl_MultiTexCoord1 -> Coordenada de textura (u,v) de textura numero 1
// ...
// attribute vec4 gl_MultiTexCoordN -> Coordenada de textura (u,v) de textura numero N


//Entrada al VertexShader
void main()
{

	// Calculamos el vector de luz en el espacio Modelo
	vLightVector = vLightPosition - gl_Vertex.xyz;

	// Transformamos el vector de luz desde el espacio Modelo a el espacio de Textura(tangente)
	mat3 TBNMatrix = mat3(vTangent, vBinormal, vNormal);
	vLightVector.xyz = TBNMatrix * vLightVector;

	// Transformamos el vertex desde espacio Modelo hasta espacio Clip... ya que la no estar usando la fixed function pipeline
	// no se realizara automaticamente esta transformacion
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	//gl_Position = ftransform(); Otra alterativa seria usar esta linea para realizar la transformacion de arriba

	// Enviamos las coordenadas de la textura de color y la textura de normales al fragment shader
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord0;
}