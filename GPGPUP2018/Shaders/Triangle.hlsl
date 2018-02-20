
/* Un sombreador básico con salida gráfica sobre una textura 2D

Este ejemplo tiene como objetivo presentar la estructura basica de un HLSL para DirectCompute

*/

//Recursos Globales (Los recursos globales son accesibles por todos los grupos de hilos)
RWTexture2D<float4> Output;  //Recurso de salida global de tipo Textura2D
cbuffer PARAMS
{
	float4 Color;
	float4 BackGround;
};


float pendientePunto(float x_1, float y_1, float x_2, float y_2, float puntox, float puntoy) {

	float pendiente = (y_2 - y_1) / (x_2 - x_1);
	return floor(pendiente*puntox - pendiente * x_1 + y_1 - puntoy);

}
//Recursos Locales (información que compartirán los hilos de éste grupo)
//<declarar aquí recursos locales>


//Estructura del grupo de hilos, un grupo de hilos comparte información entre hilos, no entre grupos.
[numthreads(32, 16, 1)]        //Un grupo de hilos que consiste en 16x16x1 = 256 hilos
void main(uint3 id:SV_DispatchThreadID) //Cada hilo recibe su IDentificador de hilo
{

	float radius = pow(id.x - 320.0f, 2) + pow(id.y - 260.0f, 2);

	float ladoAB = pendientePunto(200.0, 360.0, 320.0, 80.0, id.x, id.y);
	float ladoBC = pendientePunto(320.0, 80.0, 440.0, 360.0, id.x, id.y);
	float ladoCA = pendientePunto(440.0, 360.0, 200.0, 360.0, id.x, id.y);


	float4 blanco = float4(1, 1, 1, 1);
	float4 negro = float4(0, 0, 0, 1);

	Output[id.xy] = BackGround;
	if (ladoAB <= 0.0  && ladoBC <= 0.0 && ladoCA >= 0.0)
		Output[id.xy] = blanco;


	if (radius <= 1200.0)
		Output[id.xy] = Color;


}

