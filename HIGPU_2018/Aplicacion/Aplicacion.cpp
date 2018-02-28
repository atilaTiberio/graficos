#include "stdafx.h"
#include "CDXManager.h"
#include "resource.h"
#include "CParticleDynamics.h"
#include "CVectorReduction.h"
#include "PrimerExamen.h"
#include <fstream>
#include <iterator>
 //1.- Registrar una clase de ventana 
 //2.- Crear una ventana
 //3.- Escribir el código de un procedimiento ventana
 //4.- Implementar el bucle de mensajes

ATOM RegistrarClasesVentana(HINSTANCE hInstance);

HWND CrearVentana(HINSTANCE hInstance);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParam);
HINSTANCE g_hInstance;
CDXManager g_Manager;

CParticleDynamics g_ParticleDynamics(&g_Manager);
CVectorReduction g_VectorReduction(&g_Manager);
PrimerExamen primerExamen(&g_Manager);

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
	switch (msg)
	{
	case WM_CREATE: //1 vez y se produce antes de ser visible la vent.
		SetTimer(hWnd, 1, 10, 0);
		return 0; //OK
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			InvalidateRect(hWnd, 0, 0);
			break;
		}
		break;
	case WM_PAINT:
	{
		/*
		*/

		ID3D11Texture2D* pBackBuffer = nullptr;
		ID3D11RenderTargetView *pRTVBackBuffer = nullptr;
		ID3D11UnorderedAccessView* pUAVBackBuffer = nullptr;
		g_Manager.GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
		g_Manager.GetDevice()->CreateUnorderedAccessView(pBackBuffer, 0, &pUAVBackBuffer);
		g_Manager.GetDevice()->CreateRenderTargetView(pBackBuffer, 0, &pRTVBackBuffer);
		float Color[4] = { 0,0,0,0 };
		g_Manager.GetContext()->ClearRenderTargetView(pRTVBackBuffer, Color);
		
		/*g_ParticleDynamics.Run();
		g_ParticleDynamics.Draw(pUAVBackBuffer);
		*/

		pBackBuffer->Release();
		pRTVBackBuffer->Release();
		pUAVBackBuffer->Release();
		g_Manager.GetSwapChain()->Present(1, 0);
	}
	ValidateRect(hWnd, NULL);
	break;
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
	
	
	g_hInstance = hInstance;
	RegistrarClasesVentana(hInstance);
	HWND hWnd = CrearVentana(hInstance);

	/**/
	DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_CHOOSE_DEVICE), hWnd, ChooseDeviceDlg);



	if (!g_pAdapter)
		return 0;


	if (!g_Manager.Initialize(hWnd, g_pAdapter, 640, 480))
		return 0;

	/*
	if (!g_ParticleDynamics.Initialize(1024))
		return 0;

	if (!g_VectorReduction.Initialize(1000)) {
		return 0;
	}

	for (int i = 1; i <= 1000; i++) {
		g_VectorReduction.m_Vector[i - 1] = i;
	}
	char szMessage[1024];
	sprintf_s(szMessage, "%f", g_VectorReduction.Run());
		MessageBoxA(NULL, szMessage, "Resultado", MB_ICONINFORMATION);

		*/

	

	primerExamen.InitializeAnsiText();
	bool archivoModificado = primerExamen.RunAnsi();

	if (!archivoModificado) {
		MessageBoxA(NULL, "Error al generar archivo modificado", "Convertir ansi", MB_ICONINFORMATION);
	}
	else {
		MessageBoxA(NULL, "Archivo modificado exitosamente", "Convertir ansi", MB_ICONINFORMATION);
	}

	primerExamen.InitializeSuzanne();
	VERTEXINFO res = primerExamen.Run();

	char szMessage[1024];
	sprintf_s(szMessage, "Centroide en X: %f, Y: %f, Z: %f, W: %f", res.x, res.y, res.z, res.w);

	MessageBoxA(NULL, szMessage, "Mesh Suzanne", MB_ICONINFORMATION);
	

	
	



	/*
	
	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	while (true)
	{
		BOOL bContinue = GetMessage(&msg, 0, 0, 0);
		if (!bContinue) break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;

	*/
}
