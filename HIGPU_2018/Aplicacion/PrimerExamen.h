#pragma once
#include<vector>
#include "CDXManager.h"
#include <vector>
using namespace std;
struct VERTEXINFO {
	float x;
	float y;
	float z;
	float w;
};

struct ANSI {
	int x;
};

class PrimerExamen
{

	CDXManager *m_pDXM;
	ID3D11ComputeShader *m_pCS; //Para la reducción de los vertices del mesh de Suzanne
	ID3D11Buffer *m_pCB;
	ID3D11UnorderedAccessView * m_pUAVVector;
public:
	vector<VERTEXINFO> m_Vector;
	vector<ANSI> m_VectorTexto;
	PrimerExamen(CDXManager *pOwner);
	~PrimerExamen();
	bool InitializeSuzanne();
	VERTEXINFO Run();
	bool loadVertices();

	bool InitializeAnsiText();
	bool RunAnsi();
	bool loadAnsiText();
	long getFileSize(FILE *file);


};


