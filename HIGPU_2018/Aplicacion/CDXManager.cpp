#include "stdafx.h"
#include "CDXManager.h"
#include "resource.h"
#include <d3dcompiler.h>


IDXGIAdapter * CDXManager::EnumAndChooseAdapter(HWND hWnd)
{
	IDXGIFactory* pDXGIFactory = nullptr;
	CreateDXGIFactory(IID_IDXGIFactory, (void**)&pDXGIFactory);
	IDXGIAdapter* pDXGIAdapter = nullptr;
	unsigned long iAdapter = 0;
	while (1)
	{
		DXGI_ADAPTER_DESC dad;
		pDXGIFactory->EnumAdapters(iAdapter, &pDXGIAdapter);
		if (!pDXGIAdapter) break;
		pDXGIAdapter->GetDesc(&dad);
		wchar_t szMessage[1024];
		wsprintf(szMessage, L"Descripción:%s\r\nMemoria Dedicada:%d MB",
			dad.Description, dad.DedicatedVideoMemory / (1024 * 1024));
		switch (MessageBox(hWnd, szMessage, L"Selección de dispositivo",
			MB_ICONQUESTION | MB_YESNOCANCEL))
		{
		case IDYES:
			pDXGIFactory->Release();
			return pDXGIAdapter;
		case IDNO:
			pDXGIAdapter->Release();
			iAdapter++;
			break;
		case IDCANCEL:
			pDXGIAdapter->Release();
			pDXGIFactory->Release();
			return nullptr;
		}
	}
	return nullptr;
}

CDXManager::CDXManager()
{
}


CDXManager::~CDXManager()
{
}


bool CDXManager::Initialize(HWND hWnd, IDXGIAdapter* pAdapter, int sx, int sy,bool b_UseSoftware )
{
	DXGI_SWAP_CHAIN_DESC dscd;
	memset(&dscd, 0, sizeof(dscd));
	dscd.BufferCount = 2;
	dscd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dscd.BufferDesc.Height = sy;
	dscd.BufferDesc.Width = sx;
	dscd.BufferDesc.RefreshRate.Numerator = 0;
	dscd.BufferDesc.RefreshRate.Denominator = 0;
	dscd.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	dscd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	dscd.OutputWindow = hWnd;
	dscd.BufferUsage =
		DXGI_USAGE_RENDER_TARGET_OUTPUT |
		DXGI_USAGE_UNORDERED_ACCESS;
	dscd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	
	dscd.SampleDesc.Count = 1;
	dscd.SampleDesc.Quality = 0;
	dscd.Windowed = true;
	D3D_FEATURE_LEVEL Requested = D3D_FEATURE_LEVEL_11_0, Serviced;
	HRESULT hr ;

	if (!b_UseSoftware) {
		dscd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		hr = D3D11CreateDeviceAndSwapChain(pAdapter,
			D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, &Requested, 1,
			D3D11_SDK_VERSION, &dscd,
			&m_pSwapChain, &m_pDevice, &Serviced, &m_pContext);
	}
	else {
		dscd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		hr = D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_WARP, NULL, 0, &Requested, 1,
			D3D11_SDK_VERSION, &dscd,
			&m_pSwapChain, &m_pDevice, &Serviced, &m_pContext);
	}



	return SUCCEEDED(hr);
}


ID3D11ComputeShader* CDXManager::CompileCS(const wchar_t * pwszFileName, const char * pszEntryPoint)
{
	//D3DBlob (D3D Binary Large Object = Buffer, Arreglo, Bloque de memoria)
	ID3DBlob* pDXIL = nullptr; //Lenguaje Intermedio de DirectX (resultado de la compilación)
	ID3DBlob* pErrors = nullptr; //Errores y advertencias resultado de la compilación
#ifdef _DEBUG
								 //Banderas de compilación depurable
	int Flags = D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
								 //Banderas de compilación optimizada (no depurable)
	int Flags = D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_ENABLE_STRICTNESS;
#endif
	//Compilar el archivo de HLSL a DXIL
	HRESULT hr = D3DCompileFromFile(pwszFileName,
		NULL, //Sin definiciones previas 
		D3D_COMPILE_STANDARD_FILE_INCLUDE, //Sistema de pila de inclusión de archivos estándar
		pszEntryPoint,  //
		"cs_5_0",   //Shader Model de Compute Shader 5.0 
		Flags, 0,
		&pDXIL, &pErrors);
	if (pErrors)
	{
		MessageBoxA(NULL, (char*)pErrors->GetBufferPointer(), "Errores y avisos generados", MB_ICONEXCLAMATION);
		pErrors->Release();
	}
	if (pDXIL)
	{
		//Compilar de DXIL a Nativo para ese GPU/CPU
		ID3D11ComputeShader* pCS = nullptr;
		m_pDevice->CreateComputeShader(pDXIL->GetBufferPointer(), pDXIL->GetBufferSize(), NULL, &pCS);
		pDXIL->Release();
		return pCS;
	}
	return nullptr;
}

void CDXManager::LoadAdaptersToListBox(HWND hDlg)
{
	IDXGIFactory* pDXGIFactory = nullptr;
	CreateDXGIFactory(IID_IDXGIFactory, (void**)&pDXGIFactory);
	IDXGIAdapter* pDXGIAdapter = nullptr;
	unsigned long iAdapter = 0;
	HWND hwndList = GetDlgItem(hDlg, IDC_LIST_DEVICES);

	while (1)
	{
		DXGI_ADAPTER_DESC dad;
		pDXGIFactory->EnumAdapters(iAdapter, &pDXGIAdapter);

		if (!pDXGIAdapter) break;

		pDXGIAdapter->GetDesc(&dad);
		wchar_t szMessage[1024];

		wsprintf(szMessage, L"%s\r\n", dad.Description);

		int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)szMessage);
		SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM)pDXGIAdapter);

		iAdapter++;

	}
	SetFocus(hwndList);
}

void CDXManager::calculaLimitesJulia(PARAMS &params)
{


	params.minReal = (-1.0f*params.zoom) + params.x_centro; //centro en X (0,0)
	params.maxReal = (1.0f*params.zoom) + params.x_centro; //centro en X (0,0)
	params.minImaginario = (-1.0f*params.zoom) + params.y_centro; //centro en Y(0,0)
	params.maxImaginario = (1.0f*params.zoom) + params.y_centro;



	params.jReal = (params.maxReal - params.minReal) / (640 - 1);
	params.jImaginaria = (params.maxImaginario - params.minImaginario) / (640 - 1);

	/*
	params.minReal = -1.0f;
	params.maxReal = 1.0f;
	params.minImaginario = -1.0f;
	params.maxImaginario = params.minImaginario + (params.maxReal - params.minImaginario)*(params.alto / params.ancho);


	params.minReal = (-1.0f*(params.alto / params.ancho)) + params.x_centro; //centro en X (0,0)
	params.maxReal = (1.0f*(params.alto / params.ancho)) + params.x_centro; //centro en X (0,0)
	params.minImaginario = (-1.0f*(params.alto / params.ancho)) + params.y_centro; //centro en Y(0,0)
	params.maxImaginario = params.minImaginario + (params.maxReal - params.minImaginario)*(params.alto / params.ancho);

	params.jReal = (params.maxReal - params.minReal) / (params.ancho - 1);
	params.jImaginaria = (params.maxImaginario - params.minImaginario) / (params.ancho -1);

	*/
}


void CDXManager::Upload(void* pData, size_t uSize, ID3D11Buffer* pDest)
{
	ID3D11Buffer* pStaging = nullptr;
	D3D11_BUFFER_DESC dbd;
	pDest->GetDesc(&dbd);
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dbd.Usage = D3D11_USAGE_STAGING;
	dbd.BindFlags = 0;
	m_pDevice->CreateBuffer(&dbd, NULL, &pStaging);
	D3D11_MAPPED_SUBRESOURCE dms;
	m_pContext->Map(pStaging, 0, D3D11_MAP_WRITE, 0, &dms);
	memcpy(dms.pData, pData, uSize);
	m_pContext->Unmap(pStaging, 0);
	m_pContext->CopyResource(pDest, pStaging);
	pStaging->Release();
}
void CDXManager::Download(void* pData, size_t uSize,
	ID3D11Buffer* pSource)
{
	ID3D11Buffer* pStaging = nullptr;
	D3D11_BUFFER_DESC dbd;
	pSource->GetDesc(&dbd);
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	dbd.Usage = D3D11_USAGE_STAGING;
	dbd.BindFlags = 0;
	m_pDevice->CreateBuffer(&dbd, NULL, &pStaging);
	m_pContext->CopyResource(pStaging, pSource);
	D3D11_MAPPED_SUBRESOURCE dms;
	m_pContext->Map(pStaging, 0, D3D11_MAP_READ, 0, &dms);
	memcpy(pData, dms.pData, uSize);
	m_pContext->Unmap(pStaging, 0);
	pStaging->Release();
}

//LoadTexture by Cornejo: returns a ID3D11Texture2D bindable as Render Target View and Shader Resorce View
#include<stdio.h>
#include<fstream>
using namespace std;
ID3D11Texture2D* CDXManager::LoadTexture(
	const char* pszFileName,		//The file name
	unsigned long MipMapLevels, //Number of mipmaps to generate, -1 automatic (int)log2(min(sizeX,sizeY))+1 levels
	ID3D11ShaderResourceView** ppSRV, //The Shader Resource View
	float(*pAlphaFunction)(float r, float g, float b),  //Operates pixel's rgb channels to build an alpha channel (normalized), can be null
	VECTOR4D(*pColorFunction)(const VECTOR4D& Color))
{
	ID3D11Device* pDev = GetDevice();
	ID3D11DeviceContext* pCtx = GetContext();
	printf("Loading %s...\n", pszFileName);
	fstream bitmap(pszFileName, ios::in | ios::binary);
	if (!bitmap.is_open())
	{
		printf("Error: Unable to open file %s\n", pszFileName);
		return NULL;
	}
	//Verificar el numeo magico de un bmp
	BITMAPFILEHEADER bfh;
	bitmap.read((char*)&bfh.bfType, sizeof(bfh.bfType));
	if (!(bfh.bfType == 'MB'))
	{
		printf("Error: Not a DIB File\n");
		return NULL;
	}
	//Leerel resto de los datos
	bitmap.read((char*)&bfh.bfSize, sizeof(bfh) - sizeof(bfh.bfType));

	BITMAPINFOHEADER bih;
	bitmap.read((char*)&bih.biSize, sizeof(bih.biSize));
	if (bih.biSize != sizeof(BITMAPINFOHEADER))
	{
		printf("Error: Unsupported DIB file format.");
		return NULL;
	}
	bitmap.read((char*)&bih.biWidth, sizeof(bih) - sizeof(bih.biSize));
	RGBQUAD Palette[256];
	unsigned long ulRowlenght = 0;
	unsigned char* pBuffer = NULL;

	ID3D11Texture2D* pTemp; //CPU memory
	D3D11_TEXTURE2D_DESC dtd;
	memset(&dtd, 0, sizeof(D3D11_TEXTURE2D_DESC));
	dtd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dtd.ArraySize = 1;
	dtd.BindFlags = 0;
	dtd.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	dtd.Height = bih.biHeight;
	dtd.Width = bih.biWidth;
	dtd.Usage = D3D11_USAGE_STAGING;
	dtd.MipLevels = min(MipMapLevels, 1 + (unsigned long)floor(log(min(dtd.Width, dtd.Height)) / log(2)));
	dtd.SampleDesc.Count = 1;
	dtd.SampleDesc.Quality = 0;

	printf("Width %d, Height:%d, %dbpp\n", bih.biWidth, bih.biHeight, bih.biBitCount);
	fflush(stdout);
	pDev->CreateTexture2D(&dtd, NULL, &pTemp);
	struct PIXEL
	{
		unsigned char r, g, b, a;
	};
	D3D11_MAPPED_SUBRESOURCE ms;
	pCtx->Map(pTemp, 0, D3D11_MAP_READ_WRITE, 0, &ms);
	char *pDestStart = (char*)ms.pData + (bih.biHeight - 1)*ms.RowPitch;
	PIXEL *pDest = (PIXEL*)pDestStart;
	switch (bih.biBitCount)
	{
	case 1: //Tarea 1bpp 2 colores
		if (bih.biClrUsed == 0)//Si se usan todos los colores, ese miembro es cero
			bitmap.read((char*)Palette, 2 * sizeof(RGBQUAD));
		else
			bitmap.read((char*)Palette, bih.biClrUsed * sizeof(RGBQUAD));
		ulRowlenght = 4 * ((bih.biBitCount*bih.biWidth + 31) / 32);
		pBuffer = new unsigned char[ulRowlenght];
		for (int y = (bih.biHeight - 1); y >= 0; y--)
		{
			bitmap.read((char*)pBuffer, ulRowlenght);
			int x = 0;
			for (unsigned long iByte = 0; iByte<ulRowlenght; iByte++)
			{
				unsigned long iColorIndex;
				unsigned char c = pBuffer[iByte];
				for (int iBit = 0; iBit<8; iBit++)
				{
					iColorIndex = ((c & 0x80) != 0);
					c <<= 1;
					pDest->r = Palette[iColorIndex].rgbRed;
					pDest->g = Palette[iColorIndex].rgbGreen;
					pDest->b = Palette[iColorIndex].rgbBlue;
					if (pAlphaFunction)
						pDest->a = (unsigned char)max(0, min(255, (int)(255 * pAlphaFunction(pDest->r / 255.0f, pDest->g / 255.0f, pDest->b / 255.0f))));
					else
						pDest->a = 0xff;
					if (pColorFunction)
					{
						VECTOR4D Color = { pDest->r*(1.0f / 255), pDest->g*(1.0f / 255), pDest->b*(1.0f / 255), pDest->a*(1.0f / 255) };
						VECTOR4D Result = pColorFunction(Color);
						pDest->r = (unsigned char)max(0, min(255, (int)(Result.r * 255)));
						pDest->g = (unsigned char)max(0, min(255, (int)(Result.g * 255)));
						pDest->b = (unsigned char)max(0, min(255, (int)(Result.b * 255)));
						pDest->a = (unsigned char)max(0, min(255, (int)(Result.a * 255)));
					}
					x++;
					pDest++;
					if (x<bih.biWidth)
						continue;
				}
			}
			pDestStart -= ms.RowPitch;
			pDest = (PIXEL*)pDestStart;
		}
		delete[] pBuffer;
		break;
	case 4: //Aquí 4 bpp 16 colores
		if (bih.biClrUsed == 0)//Si se usan todos los colores, ese miembro es cero
			bitmap.read((char*)Palette, 16 * sizeof(RGBQUAD));
		else
			bitmap.read((char*)Palette, bih.biClrUsed * sizeof(RGBQUAD));
		//Leer el bitmap
		ulRowlenght = 4 * ((bih.biBitCount*bih.biWidth + 31) / 32);
		pBuffer = new unsigned char[ulRowlenght];
		for (int y = (bih.biHeight - 1); y >= 0; y--)
		{
			bitmap.read((char*)pBuffer, ulRowlenght);
			for (int x = 0; x<bih.biWidth; x++)
			{
				//Desempacar pixeles así
				unsigned char nibble = (x & 1) ? (pBuffer[x >> 1] & 0x0f) : (pBuffer[x >> 1] >> 4);
				pDest->r = Palette[nibble].rgbRed;
				pDest->b = Palette[nibble].rgbBlue;
				pDest->g = Palette[nibble].rgbGreen;
				if (pAlphaFunction)
					pDest->a = (unsigned char)max(0, min(255, (int)(255 * pAlphaFunction(pDest->r / 255.0f, pDest->g / 255.0f, pDest->b / 255.0f))));
				else
					pDest->a = 0xff;
				if (pColorFunction)
				{
					VECTOR4D Color = { pDest->r*(1.0f / 255), pDest->g*(1.0f / 255), pDest->b*(1.0f / 255), pDest->a*(1.0f / 255) };
					VECTOR4D Result = pColorFunction(Color);
					pDest->r = (unsigned char)max(0, min(255, (int)(Result.r * 255)));
					pDest->g = (unsigned char)max(0, min(255, (int)(Result.g * 255)));
					pDest->b = (unsigned char)max(0, min(255, (int)(Result.b * 255)));
					pDest->a = (unsigned char)max(0, min(255, (int)(Result.a * 255)));
				}
				pDest++;
			}
			pDestStart -= ms.RowPitch;
			pDest = (PIXEL*)pDestStart;
		}
		delete[] pBuffer;
		break;
	case 8: //Tarea 8 bpp 256 colores
		if (bih.biClrUsed == 0)//Si se usan todos los colores, ese miembro es cero
			bitmap.read((char*)Palette, 256 * sizeof(RGBQUAD));
		else
			bitmap.read((char*)Palette, bih.biClrUsed * sizeof(RGBQUAD));

		ulRowlenght = 4 * ((bih.biBitCount*bih.biWidth + 31) / 32);
		pBuffer = new unsigned char[ulRowlenght];

		for (int y = (bih.biHeight - 1); y >= 0; y--)
		{
			bitmap.read((char*)pBuffer, ulRowlenght);
			for (int x = 0; x<bih.biWidth; x++)
			{
				//Desempacar pixeles así
				unsigned char nibble = (x & 1) ? (pBuffer[x] & 0xff) : (pBuffer[x]);
				pDest->r = Palette[nibble].rgbRed;
				pDest->b = Palette[nibble].rgbBlue;
				pDest->g = Palette[nibble].rgbGreen;
				if (pAlphaFunction)
					pDest->a = (unsigned char)max(0, min(255, (int)(255 * pAlphaFunction(pDest->r / 255.0f, pDest->g / 255.0f, pDest->b / 255.0f))));
				else
					pDest->a = 0xff;
				if (pColorFunction)
				{
					VECTOR4D Color = { pDest->r*(1.0f / 255), pDest->g*(1.0f / 255), pDest->b*(1.0f / 255), pDest->a*(1.0f / 255) };
					VECTOR4D Result = pColorFunction(Color);
					pDest->r = (unsigned char)max(0, min(255, (int)(Result.r * 255)));
					pDest->g = (unsigned char)max(0, min(255, (int)(Result.g * 255)));
					pDest->b = (unsigned char)max(0, min(255, (int)(Result.b * 255)));
					pDest->a = (unsigned char)max(0, min(255, (int)(Result.a * 255)));
				}
				pDest++;
			}
			pDestStart -= ms.RowPitch;
			pDest = (PIXEL*)pDestStart;
		}
		delete[] pBuffer;
		break;
	case 24://Aquí 16777216 colores (True Color)
			//Leer el bitmap
		ulRowlenght = 4 * ((bih.biBitCount*bih.biWidth + 31) / 32);
		pBuffer = new unsigned char[ulRowlenght];
		for (int y = (bih.biHeight - 1); y >= 0; y--)
		{
			bitmap.read((char*)pBuffer, ulRowlenght);
			for (int x = 0; x<bih.biWidth; x++)
			{
				//Desempacar pixeles así
				pDest->b = pBuffer[3 * x + 0];
				pDest->g = pBuffer[3 * x + 1];
				pDest->r = pBuffer[3 * x + 2];
				if (pAlphaFunction)
					pDest->a = (unsigned char)max(0, min(255, (int)(255 * pAlphaFunction(pDest->r / 255.0f, pDest->g / 255.0f, pDest->b / 255.0f))));
				else
					pDest->a = 0xff;
				if (pColorFunction)
				{
					VECTOR4D Color = { pDest->r*(1.0f / 255), pDest->g*(1.0f / 255), pDest->b*(1.0f / 255), pDest->a*(1.0f / 255) };
					VECTOR4D Result = pColorFunction(Color);
					pDest->r = (unsigned char)max(0, min(255, (int)(Result.r * 255)));
					pDest->g = (unsigned char)max(0, min(255, (int)(Result.g * 255)));
					pDest->b = (unsigned char)max(0, min(255, (int)(Result.b * 255)));
					pDest->a = (unsigned char)max(0, min(255, (int)(Result.a * 255)));
				}
				pDest++;
			}
			pDestStart -= ms.RowPitch;
			pDest = (PIXEL*)pDestStart;
		}
		delete[] pBuffer;
		break;
	case 32:
		ulRowlenght = 4 * ((bih.biBitCount*bih.biWidth + 31) / 32);
		pBuffer = new unsigned char[ulRowlenght];
		for (int y = (bih.biHeight - 1); y >= 0; y--)
		{
			bitmap.read((char*)pBuffer, ulRowlenght);
			for (int x = 0; x<bih.biWidth; x++)
			{
				//Desempacar pixeles así
				pDest->b = pBuffer[4 * x + 0];
				pDest->g = pBuffer[4 * x + 1];
				pDest->r = pBuffer[4 * x + 2];
				if (pAlphaFunction)
					pDest->a = (unsigned char)max(0, min(255, (int)(255 * pAlphaFunction(pDest->r / 255.0f, pDest->g / 255.0f, pDest->b / 255.0f))));
				else
					pDest->a = 0xff;
				if (pColorFunction)
				{
					VECTOR4D Color = { pDest->r*(1.0f / 255), pDest->g*(1.0f / 255), pDest->b*(1.0f / 255), pDest->a*(1.0f / 255) };
					VECTOR4D Result = pColorFunction(Color);
					pDest->r = (unsigned char)max(0, min(255, (int)(Result.r * 255)));
					pDest->g = (unsigned char)max(0, min(255, (int)(Result.g * 255)));
					pDest->b = (unsigned char)max(0, min(255, (int)(Result.b * 255)));
					pDest->a = (unsigned char)max(0, min(255, (int)(Result.a * 255)));
				}
				pDest++;
			}
			pDestStart -= ms.RowPitch;
			pDest = (PIXEL*)pDestStart;
		}
		delete[] pBuffer;
		break;
	}
	//transfer cpu mem to gpu memory
	pCtx->Unmap(pTemp, 0);
	//Crear buffer en GPU
	dtd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	dtd.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	dtd.Usage = D3D11_USAGE_DEFAULT;
	dtd.CPUAccessFlags = 0;
	ID3D11Texture2D* pTexture;
	pDev->CreateTexture2D(&dtd, NULL, &pTexture);
	//copy gpu mem to gpu mem for RW capable surface
	pCtx->CopyResource(pTexture, pTemp);
	if (ppSRV)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		srvd.Texture2D.MipLevels = dtd.MipLevels;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Format = dtd.Format;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		pDev->CreateShaderResourceView(pTexture, &srvd, ppSRV);
		printf("Generating %d mipmaps levels... ", dtd.MipLevels);
		fflush(stdout);
		pCtx->GenerateMips(*ppSRV);
		printf("done.\n");
		fflush(stdout);
	}
	pTemp->Release();
	printf("Load success.\n");
	return pTexture;
}

