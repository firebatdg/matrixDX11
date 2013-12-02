////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include "Buffers.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
public:
	struct IndexType
	{
		WORD v[3];
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
	    D3DXVECTOR4 color;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 UV;
	};

	

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	
	int GetVertexCount();
	int GetInstanceCount();
	int GetIndexCount();

	int GetInstanceDataTypeSize();


	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer* m_instanceBuffer;
	ID3D11Buffer* m_instanceBufferOut;
	ID3D11Buffer* m_indexBuffer;

	InstanceType* m_instances;

private:
	virtual bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	virtual void RenderBuffers(ID3D11DeviceContext*);

protected:

	int m_vertexCount;
	int m_instanceCount;
	int m_indexCount;
};

#endif