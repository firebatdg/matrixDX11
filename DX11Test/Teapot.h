#pragma once
#include "modelclass.h"

class Teapot :
	public ModelClass
{
public:
	Teapot(void);
	~Teapot(void);
	virtual bool InitializeBuffers(ID3D11Device*);
	virtual void RenderBuffers(ID3D11DeviceContext*);

private:

	void LoadVertexFromFile(float scale);
	void LoadIndFromFile(void);
	void LoadNormalsFromFile(void);


	IndexType* indices;
	VertexType* vertices;
	InstanceType* instances;
};

