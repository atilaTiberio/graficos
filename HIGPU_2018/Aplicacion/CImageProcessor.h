#pragma once
#include "Matrix4D.h"
#include "CDXManager.h"

class CImageProcessor
{

protected:
	CDXManager * m_pDXM;
	ID3D11ComputeShader *m_pCS;
	ID3D11Buffer *m_pCB;

public:
	struct PARAMS {
		/*
		Programacion sin bifucarcion en el gpu
		*/
		MATRIX4D M; //Matrix de transformacion lineal, si solo M Offset=0 Modulate=1
		VECTOR4D Offset; //desplazamiento de color
		VECTOR4D Modulate; //modulacion de color, de 0 a 1 la multiplicacion no escapa de estos valores
	}m_Params;

	ID3D11ShaderResourceView  *m_pSRVInput;
	ID3D11UnorderedAccessView *m_pUAVOuput;
	CImageProcessor(CDXManager *pOwner);
	
	bool Initialize();
	void Uninitialize();
	void UpdateCB();
	void Run();

	~CImageProcessor();
};

