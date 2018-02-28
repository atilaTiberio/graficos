struct ANSI {
	int ansicode;
};
int isLetra(int val) {

	if ((val >= 97 && val <= 122) || (val >= 224 && val <= 246))
		return val - 32;
	else
		return val;
}

RWStructuredBuffer<ANSI> Vector;
[numthreads(64, 1, 1)]
void ConvertANSI(uint id:SV_DispatchThreadID, uint3 lid : SV_GroupThreadID, uint3 grpid : SV_GroupID)
{ 
	/*
	En el int entraron 4 chars, esto unicamente será funcional para ANSI, 
	si el archivo está en UTF-8 o en algún otro formato, esto 
	no funcionará del todo bien, se deberá usar otro valor de mascará

	Fue más rápido procesarlo de este modo, ya que haber leido char por char y crear el int desde el cpu para despues mandarlo al gpu
	era demasiado lento 
  */
	
	int num = Vector[id.x].ansicode;

	int a = isLetra((num >> 24) & 0xFF);
	int b = isLetra((num >> 16) & 0xFF);
	int c = isLetra((num >> 8) & 0xFF);
	int d = isLetra(num & 0xFF);

	num = (a << 24) | (b << 16) | (c << 8) | d;
	
	Vector[id.x].ansicode = num;


}