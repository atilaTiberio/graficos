#include "stdafx.h"
#include "CImageProcessor.h"

CImageProcessor::CImageProcessor(CDXManager * pOwner)
{
	m_pDXM = pOwner;
	m_pCB = nullptr;
	m_pCS = nullptr;
}

bool CImageProcessor::Initialize()
{
	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbd.Usage = D3D11_USAGE_DYNAMIC;
	dbd.ByteWidth = 16 * ((sizeof(PARAMS) + 15) / 16);
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	/*
		el buffer estructurado se usa el StructureByteStride
	*/
	HRESULT res=m_pDXM->GetDevice()->CreateBuffer(&dbd, 0, &m_pCB);

	m_pCS = m_pDXM->CompileCS(L"..\\Shaders\\ImageProcessor.hlsl", "main");

	if (!m_pCB)
		return false;

	if (!m_pCS) {
		m_pCB->Release();
		m_pCB = nullptr;
		return false;
	}

	m_Params.M = Identity();
	m_Params.Offset = { 0, 0, 0, 0 };
	m_Params.Modulate = { 1,1,1,1 };


	return false;
}

void CImageProcessor::UpdateCB()
{
	D3D11_MAPPED_SUBRESOURCE ms;
	/*
		para los andamios es write sin dscard
		el segundo 0 espera hastra que este disponible
	*/
	m_pDXM->GetContext()->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD,0,&ms);
	
	PARAMS Temp = m_Params;
	Temp.M = Transpose(m_Params.M);
	memcpy(ms.pData, &Temp, sizeof(PARAMS));
	m_pDXM->GetContext()->Unmap(m_pCB, 0);



}

void CImageProcessor::Uninitialize()
{
}


void CImageProcessor::Run()
{
	auto pCtx = m_pDXM->GetContext();

	pCtx->CSSetShader(m_pCS, 0, 0);
	/*
	m_pUAVOuput se conoce las dimensiones de la salida, 
	*/
	pCtx->CSSetUnorderedAccessViews(0, 1, &m_pUAVOuput, 0);
	pCtx->CSSetConstantBuffers(0, 1, &m_pCB);
	pCtx->CSGetShaderResources(0, 1, &m_pSRVInput);
	ID3D11Resource* pResource=nullptr;
	ID3D11Texture2D* pTexture2D=nullptr;

	m_pUAVOuput->GetResource(&pResource);
	pResource->QueryInterface(IID_ID3D11Texture2D,(void**)&pTexture2D);
	D3D11_TEXTURE2D_DESC dtd;
	pTexture2D->GetDesc(&dtd);
	UpdateCB();
	/*
	Se pone de 32 que no se usara memoria de grupo compartida
	*/
	pCtx->Dispatch((dtd.Width + 31) / 32, (dtd.Height + 31) / 32, 1);
	
	pTexture2D->Release();
	pResource->Release();
	
}

CImageProcessor::~CImageProcessor()
{
}


