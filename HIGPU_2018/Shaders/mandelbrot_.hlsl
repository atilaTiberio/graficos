
/* Un sombreador básico con salida gráfica sobre una textura 2D

Este ejemplo tiene como objetivo presentar la estructura basica de un HLSL para DirectCompute

*/

//Recursos Globales (Los recursos globales son accesibles por todos los grupos de hilos)
RWTexture2D<float4> Output;  //Recurso de salida global de tipo Textura2D

cbuffer PARAMS
{
	float zoom ; // modificable por la aplicacion
	float minReal;
	float maxReal;
	float minImaginario;
	float maxImaginario; //se calculara con la escala de zoom
	float jReal; // modificable por la aplicacion
	float jImaginaria; // modificable por la aplicacion
	int maxIteraciones; // modificable por la aplicacion
	int ancho; // para el coloreado
};



//Recursos Locales (información que compartirán los hilos de éste grupo)
//<declarar aquí recursos locales>


//Estructura del grupo de hilos, un grupo de hilos comparte información entre hilos, no entre grupos.
[numthreads(32, 16, 1)]        //Un grupo de hilos que consiste en 16x16x1 = 256 hilos
void main(uint3 id:SV_DispatchThreadID) //Cada hilo recibe su IDentificador de hilo
{
	float x = id.x;
	float y = id.y;

	int nIteration = 0;


	float cIm = maxImaginario - y * jImaginaria;
	float cRe = minReal + x * jReal;

	float zRe = cRe;
	float zIm = cIm;
	float4 blanco = float4(1,1,1, 1);
	float4 mm = float4(0.5, 0.5, 0.5, 1);

	float4 negro = float4(0, 0, 0, 1);
	
		bool inside = true;
		for (nIteration = 0; nIteration < maxIteraciones; ++nIteration)
		{
			float zRe2 = zRe * zRe;
			float zIm2 = zIm * zIm;

			/// check whether absolute value of z has gotten bigger than 2
			if (zRe2 + zIm2 > 4) {
				inside = false;
				break;
			}
			// calculate next z(n)
			zIm = 2 * zRe*zIm + cIm;
			zRe = zRe2 - zIm2 + cRe;
		}

		if(inside)
			Output[id.xy] = blanco;
		
		
		

}

