struct VERTEXINFO {
	float4 vertexSuzanne;
};

groupshared VERTEXINFO Sums[64]; 
static const int BlockSize = 128;
RWStructuredBuffer<VERTEXINFO> Vector;

[numthreads(64, 1, 1)]
void Suzanne(uint id:SV_DispatchThreadID, uint3 lid : SV_GroupThreadID, uint3 grpid : SV_GroupID)
{

	Sums[lid.x].vertexSuzanne = Vector[grpid.x * 128 + lid.x].vertexSuzanne + Vector[grpid.x * 128 + 64 + lid.x].vertexSuzanne;
	GroupMemoryBarrierWithGroupSync();
	if (lid.x < 32) {
		Sums[lid.x].vertexSuzanne += Sums[lid.x + 32].vertexSuzanne;
	}
	GroupMemoryBarrierWithGroupSync();

	if (lid.x < 16) {
		Sums[lid.x].vertexSuzanne += Sums[lid.x + 16].vertexSuzanne;
	}
	GroupMemoryBarrierWithGroupSync();
	if (lid.x < 8) {
		Sums[lid.x].vertexSuzanne += Sums[lid.x + 8].vertexSuzanne;
	}
	GroupMemoryBarrierWithGroupSync();
	if (lid.x < 4) {
		Sums[lid.x].vertexSuzanne += Sums[lid.x + 4].vertexSuzanne;
	}
	GroupMemoryBarrierWithGroupSync();

	if (lid.x < 2) {
		Sums[lid.x].vertexSuzanne += Sums[lid.x + 2].vertexSuzanne;
	}
	GroupMemoryBarrierWithGroupSync();
	if (lid.x < 1) {
		Sums[lid.x].vertexSuzanne += Sums[lid.x + 1].vertexSuzanne;
		Vector[grpid.x * 128] = Sums[0];
	}
	
	
	
}