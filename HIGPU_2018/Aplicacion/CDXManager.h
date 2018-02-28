#pragma once
#pragma once
#include <dxgi.h>
#include <d3d11.h>

struct PARAMS
{
	float zoom = 1.0f; // modificable por la aplicacion
	float minReal = -1.0f;
	float maxReal = 1.0f;
	float minImaginario = -1.0f;
	float maxImaginario; //se calculara con la escala de zoom
	float jReal = -0.51f; // modificable por la aplicacion
	float jImaginaria = -0.601f; // modificable por la aplicacion
	int maxIteraciones = 255; // modificable por la aplicacion
	float ancho = 640.0f;
	float alto = 480.0f;
	float x_centro = 0.0f;
	float y_centro = 0.0f;

};

class CDXManager
{
protected:
	IDXGISwapChain * m_pSwapChain;  //Infraestructura de render
	ID3D11Device*       m_pDevice;     //Fabrica de recursos 
	ID3D11DeviceContext* m_pContext;   //Fachada funcionalidad
public:
	IDXGIAdapter * EnumAndChooseAdapter(HWND hWnd);
	IDXGISwapChain* GetSwapChain() { return m_pSwapChain; };
	ID3D11Device* GetDevice() { return m_pDevice; }
	ID3D11DeviceContext* GetContext() { return m_pContext; }
	CDXManager();
	~CDXManager();
	bool Initialize(HWND hWnd, IDXGIAdapter* pAdapter, int sx, int sy, bool b_UseSoftware = true);
	ID3D11ComputeShader* CompileCS(const wchar_t* pwszFileName, const char* pszEntryPoint);
	void LoadAdaptersToListBox(HWND hDlg);
	void calculaLimitesJulia(struct PARAMS &params);
	void Upload(void* pData, size_t uSize, ID3D11Buffer* pDest);
	void Download(void* pData, size_t uSize, ID3D11Buffer* pSource);
	
	
};

