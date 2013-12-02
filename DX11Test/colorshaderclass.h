////////////////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _COLORSHADERCLASS_H_
#define _COLORSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: ColorShaderClass
////////////////////////////////////////////////////////////////////////////////
class ColorShaderClass
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};
	struct PhongParams
	{
		D3DXVECTOR3 g_vLightPos[3];
	    D3DXVECTOR4 g_vLightColor[3];
		D3DXVECTOR4 g_Ambient;
		D3DXVECTOR4 g_Diffuse;       // Material's diffuse color
		D3DXVECTOR4 g_Specular;
		float  g_kDiffuse; 
		float  g_PhongExp;  
		D3DXVECTOR3 g_vEyePos; 
	};
public:
	ColorShaderClass();
	ColorShaderClass(const ColorShaderClass&);
	ColorShaderClass(ID3D11ShaderResourceView*);
	~ColorShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* , int , int , int,  D3DXMATRIX , D3DXMATRIX , D3DXMATRIX );
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	ID3D11ShaderResourceView* m_srv;

private:
	bool InitializeShader(ID3D11Device* , HWND, WCHAR*, LPCSTR , LPCSTR );
	void ShutdownShader();
	

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
	void RenderShader(ID3D11DeviceContext* , int , int, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	
};

#endif