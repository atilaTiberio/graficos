// Aplicacion.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CDXManager.h"
#include "resource.h"
#include <iostream>
using namespace std;
//1.- Registrar una clase de ventana 
//2.- Crear una ventana
//3.- Escribir el código de un procedimiento ventana
//4.- Implementar el bucle de mensajes
#define UP_ARROW    72
#define LEFT_ARROW  75
#define DOWN_ARROW  80
#define RIGHT_ARROW 77
ATOM RegistrarClasesVentana(HINSTANCE hInstance);
HWND CrearVentana(HINSTANCE hInstance);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, 
	WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ChooseDeviceDlg(HWND hDlg,
	UINT msg,
	WPARAM wParam, LPARAM lParam);
HINSTANCE g_hInstance;
CDXManager g_Manager;
ID3D11ComputeShader* g_pCS;
IDXGIAdapter * g_pAdapter;
ID3D11Buffer*        g_pCB;
PARAMS g_Params;


INT_PTR CALLBACK ChooseDeviceDlg(HWND hDlg,
	UINT msg,
	WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		g_Manager.LoadAdaptersToListBox(hDlg);

		return TRUE; //Set Focus to first IDC
	}
	case WM_CLOSE:
		EndDialog(hDlg, IDCLOSE);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
		case IDC_LIST_DEVICES:
		{
			switch (HIWORD(wParam))
			{
			case LBN_SELCHANGE:
			{
				HWND hwndList = GetDlgItem(hDlg, IDC_LIST_DEVICES);
				IDXGIAdapter* pDXGIAdapter = nullptr;
				// Get selected index.
				int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);

				// Get item data.
				pDXGIAdapter = (IDXGIAdapter*)SendMessage(hwndList, LB_GETITEMDATA, lbItem, 0);

				DXGI_ADAPTER_DESC dad;
				pDXGIAdapter->GetDesc(&dad);
				wchar_t szMessage[1024];

				wsprintf(szMessage, L"\r\nDescripción:%s\r\nMemoria Dedicada:%d MB \r\n",
					dad.Description, dad.DedicatedVideoMemory / (1024 * 1024));
				SetDlgItemText(hDlg, IDC_STATIC_DEVICE_SPECS, szMessage);
				g_pAdapter = pDXGIAdapter;
				return TRUE;
			}
			}

		}
		return TRUE;
		}

	}
	return 0;
}

ATOM RegistrarClasesVentana(HINSTANCE hInstance)
{
	WNDCLASSEX wnc;
	memset(&wnc, 0, sizeof(wnc));
	wnc.cbSize = sizeof(wnc);
	wnc.hInstance = hInstance;
	wnc.lpfnWndProc = WndProc;
	wnc.lpszClassName = L"MiVentana";
	wnc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	return RegisterClassEx(&wnc);
}
HWND CrearVentana(HINSTANCE hInstance)
{
	HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MiVentana",
		L"Mi Primer ventana en C++", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL, hInstance, NULL);
	return hWnd;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, 
					   WPARAM wParam, LPARAM lParam)
{
	//wParam parámetros de valor (generalmente)
	//lParam parametros de valor y generalmente referencias
	float factor = 0.0f;
	switch (msg)
	{
	case WM_CREATE: //1 vez y se produce antes de ser visible la vent.
		return 0; //OK
	case WM_PAINT:
		{
			//0.- Actualizar constant buffer
			D3D11_MAPPED_SUBRESOURCE ms;
			g_Manager.GetContext()->Map(g_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
			memcpy(ms.pData, &g_Params, sizeof(PARAMS));
			g_Manager.GetContext()->Unmap(g_pCB, 0);
			g_Manager.GetContext()->CSSetConstantBuffers(0, 1, &g_pCB);
			//1.- Solicitar acceso al back buffer, objetivo de los resultados del compute shader.
			ID3D11Texture2D* pBackBuffer = nullptr;
			g_Manager.GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
			//2.- Crear una vista de recurso para asociar el back buffer con el sombreador de computación
			ID3D11UnorderedAccessView* pUAV = nullptr; //Vista de recurso de acceso no ordenado
			g_Manager.GetDevice()->CreateUnorderedAccessView(
				pBackBuffer, //Recurso a crear su vista 
				NULL, //igual que el formato del recurso (solo si el recurso ya tiene formato)  
				&pUAV); //Vista de acceso no ordenado al backbuffer (resultado)
			//3.-Configurar la tubería de DirectCompute
			//3.1.- Instalar el sombreador
			g_Manager.GetContext()->CSSetShader(g_pCS, 0, 0);
			//3.2.- Instalar la salida (la vista UAV)
			g_Manager.GetContext()->CSSetUnorderedAccessViews(
				0, //Salida 0
				1, //1 vista
				&pUAV, //La vista
				0); //Contadores iniciales nulos, aun sin usar
			//4.- Ejecución del sombreador con la tubería configurada en el paso 3
			g_Manager.GetContext()->Dispatch(640 / 32, 480 / 16, 1);
            //5.- Mostrar el resultado en ventana
			g_Manager.GetSwapChain()->Present(2, 0);
			//6.- Liberar recursos temporales
			pUAV->Release();
			pBackBuffer->Release();
	}
		ValidateRect(hWnd, NULL);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			g_Params.y_centro -= 0.00003f*3.0f/g_Params.zoom;
			g_Manager.calculaLimitesJulia(g_Params);
			InvalidateRect(hWnd, NULL, false);
			break;
		case VK_DOWN:
			g_Params.y_centro += 0.00003f*3.0f / g_Params.zoom;
			g_Manager.calculaLimitesJulia(g_Params);
			InvalidateRect(hWnd, NULL, false);
			break;
		case VK_LEFT:
			g_Params.x_centro -= 0.00003f*3.0f / g_Params.zoom;
			g_Manager.calculaLimitesJulia(g_Params);
			InvalidateRect(hWnd, NULL, false);
			break;
		case VK_RIGHT:
			g_Params.x_centro += 0.00003f*3.0f / g_Params.zoom;
			g_Manager.calculaLimitesJulia(g_Params);
			InvalidateRect(hWnd, NULL, false);
			break;
		case 'Z':
			//g_Params.maxIteraciones += 50;
			g_Params.zoom *= pow(1.01f,3.0f);
			g_Manager.calculaLimitesJulia(g_Params);
			InvalidateRect(hWnd, NULL, false);
			break;
		case 'X':
			g_Params.zoom /= pow(1.01f, 3.0f);
			g_Manager.calculaLimitesJulia(g_Params);
			InvalidateRect(hWnd, NULL, false);
			break;

		case 'I': //Incrementa constante imaginaria
			g_Params.constanteIm += 0.0002f * 3 / g_Params.zoom;
			InvalidateRect(hWnd, NULL, false);
			break;
		case 'O': //Reduce constante imaginaria
			g_Params.constanteIm -= 0.0002f * 3 / g_Params.zoom;
			InvalidateRect(hWnd, NULL, false);
			break;
		case 'R': //Incrementa constante real
			g_Params.constanteRe += 0.0002f * 3 / g_Params.zoom;
			InvalidateRect(hWnd, NULL, false);
			break;
		case 'E': //Incrementa constante real
			g_Params.constanteRe -= 0.0002f* 3 / g_Params.zoom;
			InvalidateRect(hWnd, NULL, false);
			break;
		case 'M':
			g_Params.maxIteraciones *= 2;
			InvalidateRect(hWnd, NULL, false);
			break;
		case 'N':
			if(g_Params.maxIteraciones>2)
				g_Params.maxIteraciones /= 2;
			InvalidateRect(hWnd, NULL, false);
			break;

		}
		return 0;
	case WM_DESTROY: //1 vez, inminente
		PostQuitMessage(0);
		break;
	case WM_CLOSE: //Cada vez que se presiona el boton cerrar
		if (IDYES == MessageBox(hWnd,
			L"¿Desea realmente salir?", L"Salir",
			MB_YESNO | MB_ICONQUESTION))
		{
			DestroyWindow(hWnd);
		}
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(
	HINSTANCE hInstance,     //Credencial de aplicación 
	HINSTANCE hPrevInstance, //0 Obsoleto
	char* pszCmdLine,        //Linea de comandos, parametros inicio
	int nCmdShow)		     //Comando de muestra (min,max,rst)
{
	g_hInstance = hInstance;
	//IDXGIAdapter * pAdapter = g_Manager.EnumAndChooseAdapter(NULL);
	
	g_Manager.calculaLimitesJulia(g_Params);
	
	

	RegistrarClasesVentana(hInstance);
	HWND hWnd = CrearVentana(hInstance);
	DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_CHOOSE_DEVICE), hWnd, ChooseDeviceDlg);
	


	if (!g_pAdapter)
		return 0;

	if (!g_Manager.Initialize(hWnd, g_pAdapter, 640, 480))
		return 0;
	g_pCS = g_Manager.CompileCS(L"..\\Shaders\\Julia.hlsl", "main");
	//Creación de un constant buffer
	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbd.ByteWidth = 16 * ((sizeof(PARAMS) + 15) / 16);
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dbd.Usage = D3D11_USAGE_DYNAMIC;
	g_Manager.GetDevice()->CreateBuffer(&dbd, NULL, &g_pCB);
	if (!g_pCS)
		return 0;
	ShowWindow(hWnd,nCmdShow);
	MSG msg;
	while(true)
	{
		BOOL bContinue=GetMessage(&msg, 0, 0, 0);
		if (!bContinue) break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
