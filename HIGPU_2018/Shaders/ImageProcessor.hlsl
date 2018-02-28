
Texture2D<float4> Input:register(s0); //shader resource
RWTexture2D<float4> Output:register(u0);

cbuffer PARAMS:register(b0) {
	matrix M;
	float4 Offset;
	float4 Modulate;
};

[numthreads(32,32,1)]
void main(uint3 id:SV_DispatchThreadID)  
{
	float4 Position = float4(id.xy, 0, 1);
	//float4 InputPosition = mul(Position, M);
	int4 InputPosition = (int4) (mul(Position, M));
	Output[id.xy] = Input[InputPosition.xy] * Modulate + Offset;
	
}