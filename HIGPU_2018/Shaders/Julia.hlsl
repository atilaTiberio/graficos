
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
	float x_step; // modificable por la aplicacion
	float y_step; // modificable por la aplicacion
	uint maxIteraciones; // modificable por la aplicacion
	uint ancho; // para el coloreado
	float constanteRe ;
	float constanteIm ;
	float alto;
	float x_centro;
	float y_centro;
};



//Recursos Locales (información que compartirán los hilos de éste grupo)
//<declarar aquí recursos locales>

float4 color(float nIteration) {

	float t = nIteration / maxIteraciones;

	float r = (9 * (1 - t)*t*t*t);
	float g = (15 * (1 - t)*(1 - t)*t*t);
	float b = (8.5f*(1 - t)*(1 - t)*(1 - t)*t);

	return float4(r, g, b, 1);
}

//Estructura del grupo de hilos, un grupo de hilos comparte información entre hilos, no entre grupos.
[numthreads(32, 16, 1)]        //Un grupo de hilos que consiste en 16x16x1 = 256 hilos
void main(uint3 id:SV_DispatchThreadID) //Cada hilo recibe su IDentificador de hilo
{
	float x = id.x;
	float y = id.y;

	uint nIteration = 0;


	/*
	Pixel normalizado
	*/
	

	
	float newRe = minReal + x * x_step;
	float newIm = maxImaginario - y * y_step; 
	
	
	
	
	
	/*float newRe = 0.0f;
	float newIm = 0.0f;*/
	float4 colors = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float oldRe = 0.0f;
	float oldIm = 0.0f;
		for (nIteration = 0; nIteration < maxIteraciones; ++nIteration)
		{
			oldRe = newRe;
			oldIm = newIm;
			
			
			newRe = oldRe * oldRe - oldIm * oldIm + constanteRe;
			newIm = 2 * oldRe*oldIm + constanteIm;
			
			/*newRe = oldRe * oldRe - oldIm * oldIm + pRe;
			newIm = 2 * oldRe*oldIm + pIm;*/

			/// check whether absolute value of z has gotten bigger than 2
			if ((newRe*newRe + newIm* newIm) > 4) {
					break;
			}
		}
	
		/*if (nIteration != maxIteraciones) {
		if (nIteration < maxIteraciones / 2) {
			colors = float4(nIteration / ((float)(maxIteraciones) / 2.0f), 0.0f, 0.0f, 1.0f);
			}
			else {
				float fLerp = (maxIteraciones - nIteration) / ((float)(maxIteraciones) / 2.0f);
				colors = lerp(float4(1.0f, 0.0f, 0.0f, 1.0f), float4(1.0f, 1.0f, 1.0f, 1.0f), fLerp);
			}
		}
	
		*/

		

			Output[id.xy] = color((maxIteraciones-nIteration));
		//	Output[id.xy] = float4(nIteration*cos(newRe*newRe + newIm * newIm), nIteration*sin(newRe*newRe + newIm * newIm), tan(newRe*newRe + newIm * newIm), 1.0f);
		

		//Output[id.xy] = colors;
		
		//Output[id.xy] = float4( sin(nIteration/4),1.0f, sin(nIteration/7),1.0f);

		
		

		
		
		
		

}



