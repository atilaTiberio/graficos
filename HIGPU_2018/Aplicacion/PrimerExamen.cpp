#include "stdafx.h"
#include "PrimerExamen.h"
#include <iostream>
#include <fstream>
#include<locale>

using namespace std;
PrimerExamen::PrimerExamen(CDXManager *pOwner)
{
	m_pDXM = pOwner;
}


PrimerExamen::~PrimerExamen()
{
}

bool PrimerExamen::InitializeSuzanne()
{

	if (!loadVertices())
		return false;

	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	dbd.ByteWidth = m_Vector.size() * sizeof(VERTEXINFO);
	dbd.StructureByteStride = sizeof(VERTEXINFO);
	dbd.Usage = D3D11_USAGE_DEFAULT;
	dbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	m_pDXM->GetDevice()->CreateBuffer(&dbd, NULL, &m_pCB);

	m_pCS = m_pDXM->CompileCS(L"..\\Shaders\\Suzanne.hlsl", "Suzanne");

	if (!m_pCS) {
		m_pCB->Release();
		m_pCB = nullptr;
		return false;
	}
	m_pDXM->GetDevice()->CreateUnorderedAccessView(m_pCB, NULL, &m_pUAVVector);

	return true;

}

VERTEXINFO PrimerExamen::Run()
{
	m_pDXM->Upload(&m_Vector[0], m_Vector.size() * sizeof(VERTEXINFO), m_pCB);

	m_pDXM->GetContext()->CSSetShader(m_pCS, 0, 0);
	m_pDXM->GetContext()->CSSetUnorderedAccessViews(0, 1, &m_pUAVVector, 0);
	m_pDXM->GetContext()->Dispatch((UINT)(m_Vector.size() + 63) / 64, 1, 1);

	m_pDXM->Download(&m_Vector[0], m_Vector.size() * sizeof(VERTEXINFO), m_pCB);


	VERTEXINFO res;
	memset(&res, 0, sizeof(VERTEXINFO));


	for (unsigned int i = 0; i < m_Vector.size(); i += 128) {
		res.x += m_Vector[i].x;
		res.y += m_Vector[i].y;
		res.z += m_Vector[i].z;
		res.w += m_Vector[i].w;
	}

	res.x = res.x / m_Vector.size();
	res.y = res.y / m_Vector.size();
	res.z = res.z / m_Vector.size();
	res.w = res.w / m_Vector.size();


	return res;
}

bool PrimerExamen::loadVertices()
{
	/*
	Lo interesante de este problema fue la carga del archivo binario, leer los bytes como se indica en el problema.


	*/
	const char *filePath = "..\\Recursos\\Monkey.vertex";
	unsigned long  vertexN; //N vertices

	FILE *file = NULL;    // File pointer

	if (EINVAL == fopen_s(&file, filePath, "rb"))
		return false;

	fread(&vertexN, 4, 1, file);// total de vertices primeros 4 bytes

	m_Vector.resize(vertexN);
	fread(&m_Vector[0], sizeof(VERTEXINFO), vertexN, file); // se leen los N vertices de 4 flotantes (4 bytes por flotante = 16bytes)

	fclose(file);


	return true;
}

bool PrimerExamen::InitializeAnsiText()
{

	if (!loadAnsiText()) {
		return false;
	}

	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	dbd.ByteWidth = m_VectorTexto.size()* sizeof(ANSI);
	dbd.StructureByteStride =sizeof(ANSI);
	dbd.Usage = D3D11_USAGE_DEFAULT;
	dbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	
	HRESULT res=m_pDXM->GetDevice()->CreateBuffer(&dbd, NULL, &m_pCB); // 

	m_pCS = m_pDXM->CompileCS(L"..\\Shaders\\AnsiText.hlsl", "ConvertANSI");

	if (!m_pCB)
		return false;

	if (!m_pCS) {
		m_pCB->Release();
		m_pCB = nullptr;
		return false;
	}
	m_pDXM->GetDevice()->CreateUnorderedAccessView(m_pCB, NULL, &m_pUAVVector);
	return true;
}

bool PrimerExamen::RunAnsi()
{

		m_pDXM->Upload(&m_VectorTexto[0], m_VectorTexto.size()* sizeof(ANSI), m_pCB);
		m_pDXM->GetContext()->CSSetShader(m_pCS, 0, 0);
		m_pDXM->GetContext()->CSSetUnorderedAccessViews(0, 1, &m_pUAVVector, 0);
		m_pDXM->GetContext()->Dispatch((UINT)(m_VectorTexto.size() + 63) / 64, 1, 1);
		m_pDXM->Download(&m_VectorTexto[0], m_VectorTexto.size() * sizeof(ANSI), m_pCB);

		/*
		//Mejor ahorrar tiempo de casteo, además que el archivo final, es más grande que el original

		ofstream myfile;
		myfile.open("..\\Recursos\\archivo_transformado.txt");
		const char* pointer = reinterpret_cast<const char*>(&m_VectorTexto[0]);
		size_t bytes = m_VectorTexto.size() * sizeof(ANSI);
		myfile.write(pointer, bytes);
		myfile.close();

		
		*/
	
	
		const char *filePath = "..\\Recursos\\BIBLIA_TRANSFORMADA.txt";
		FILE *file = NULL;
		if (EINVAL == fopen_s(&file, filePath, "wb"))
			return false;
		fwrite(&m_VectorTexto[0], sizeof(ANSI), m_VectorTexto.size(), file);
		fclose(file);
		
		return true;


}

bool PrimerExamen::loadAnsiText()
{
	const char *filePath = "..\\Recursos\\BIBLIA_COMPLETA.txt";


	FILE *file = NULL;   
	if (EINVAL == fopen_s(&file, filePath, "rb"))
		return false;

	long fileSize = getFileSize(file);
	UINT claimSize = (fileSize + 3) / 4;
	
	
	m_VectorTexto.resize(claimSize); /*El tamaño es en bytes, cada char en el caso de ansi es de 1 byte en un INT entran 4 bytes*/
	fread(&m_VectorTexto[0], sizeof(ANSI), claimSize, file);
	fclose(file);


	return true;
	
}

long PrimerExamen::getFileSize(FILE * file)
{
	long lCurPos, lEndPos;
	lCurPos = ftell(file);
	fseek(file, 0, 2);
	lEndPos = ftell(file);
	fseek(file, lCurPos, 0);
	return lEndPos;
}
