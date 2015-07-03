//Posicion de la luz
uniform vec3 vLightPosition;

//Vector desde fragmento hasta la luz
varying vec3 vLightVector;

//Color de la luz
uniform vec3 vLightDiffuseColor;

//Color de la iluminacion ambiente
uniform vec3 vAmbientLightColor;

//Samplers de texturas
uniform sampler2D baseTexture;
uniform sampler2D normalTexture;

//Algunas variables predefinidas que podemos llegar a utilizar:
// varying vec4 gl_TexCoord[] -> Coordenadas de textura luego de interpolacion
// vec4 gl_FragColor          -> Color de salida del fragmento del FS

void main()
{
	//Estas dos lineas no son necesarias pero hacen el codigo mas legible
	vec2 texCoords    = gl_TexCoord[0].st;
	vec2 normalCoords = gl_TexCoord[1].st;

	// Debemos normalizar el vector de la luz, ya que es linealmente interpolado atravez de la superficie
	vec3 vLightVectorNorm = normalize(vLightVector);

	// Como las normales en la textura estan en el rango [0,1] las transformamos al rango [-1,1]
	vec3 vNormalMap = 2.0 * (texture2D(normalTexture, normalCoords).rgb - 0.5);

	//Calculamos el color difuso y lo almacenamos en gl_FragColor
	// El componente difuso esta definido como: I = Al + Dl * Dm * clamp(L•N, 0, 1)
	gl_FragColor.rgb = vAmbientLightColor + vLightDiffuseColor * texture2D(baseTexture, texCoords).rgb * clamp(dot(vLightVectorNorm, vNormalMap.rgb),0.0,1.0);

	//Saturamos el color
	gl_FragColor = clamp(gl_FragColor, 0.0, 1.0);

}