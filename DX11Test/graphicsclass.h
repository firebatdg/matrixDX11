////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "Teapot.h"
#include "colorshaderclass.h"
#include "D3Dcompiler.h"
#include "PersonDetector.h"

//OBJ_LOADER
#include "OBJModel.h" //:P jiji


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

/*struct groundModel = new ObjModel();
struct material = new SurfaceMaterial();
struct textureMgr = new TextureManager();*/

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	



	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	ModelClass* m_ModelCero; // :P
	ModelClass* m_ModelUno; // :P
	ColorShaderClass* m_ColorShader;
	ID3D11ComputeShader *m_ComputeShader;
	ID3D11UnorderedAccessView* instanceZeroView;
	ID3D11UnorderedAccessView* instanceDestView;
	ID3D11ShaderResourceView* srv;
	ID3D11Buffer* m_HeightsBuffer;
	ID3D11Buffer* m_tempBuffer; //Second buffer
	ID3D11Buffer* m_PhysicsBuffer;
	ID3D11UnorderedAccessView* heightsView;
	PersonDetector* personDetector;
	int count;

};
#endif