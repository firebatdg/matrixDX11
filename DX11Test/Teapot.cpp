#include "stdafx.h"
#include "Teapot.h"
#include <stdio.h>



Teapot::Teapot(void)
{



}


Teapot::~Teapot(void)
{
}

bool Teapot::InitializeBuffers(ID3D11Device* device)
{

	this->LoadVertexFromFile(1.0f);
	this->LoadIndFromFile();
	//this->LoadNormalsFromFile();

	D3D11_BUFFER_DESC vertexBufferDesc, instanceBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, instanceData, indexData;
	HRESULT result;



	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;


	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	
	// Set the number of instances in the array.
	m_instanceCount = 4;


	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	// INSTANCING

	// Create the instance array.
	instances = new InstanceType[m_instanceCount];
	if(!instances)
	{
		return false;
	}

	// Load the instance array with data.
	instances[0].position = D3DXVECTOR3(-1.5f, -1.5f, 0.0f);
	instances[1].position = D3DXVECTOR3(-1.5f,  1.5f, 0.0f);
	instances[2].position = D3DXVECTOR3( 1.5f, 1.5f, 0.0f);
	instances[3].position = D3DXVECTOR3( 1.5f,  -1.5f, 0.0f);

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the instance array now that the instance buffer has been created and loaded.
	delete [] instances;
	instances = 0;



	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(IndexType) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;


	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}
	delete[] indices;
	indices = 0;

	return true;

}



void Teapot::RenderBuffers(ID3D11DeviceContext *deviceContext){

	unsigned int strides[3];
	unsigned int offsets[3];
	ID3D11Buffer* bufferPointers[3];

	// Set the buffer strides.
	strides[0] = sizeof(VertexType); 
	strides[1] = sizeof(InstanceType);
	strides[2] = sizeof(IndexType);

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;
	offsets[2] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_vertexBuffer;	
	bufferPointers[1] = m_instanceBuffer;
	bufferPointers[2] = m_indexBuffer;


	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 3, bufferPointers, strides, offsets);

	
	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);


    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;

}

void Teapot::LoadVertexFromFile(float scale)
{
	float x,y,z;

	vertices = new VertexType[600];
	// Load the vertices
	FILE *file = fopen(".\\Teapot3D.txt", "rt");

	if (!file) 
	{
		return;
	}


	int i=0;
	while (!feof(file)) {
		fscanf(file, "%f %f %f,\n", &x,&y,&z);
		vertices[i].position.x=x/scale;
		vertices[i].position.y=y/scale;
		vertices[i].position.z=z/scale;
		vertices[i].color = D3DXVECTOR4(1.0, 0.0, 0.0, 1.0);
		i++;
	}
	fclose(file);
	m_vertexCount = i;
	

}
//---------------------------------------------------------------------
void Teapot::LoadIndFromFile()
{

	indices = new IndexType[1500];
	// Load the triangles
	int a,b,c,dummy;
	FILE *file = fopen("TeapotTri.txt", "rt");
	if (!file) 
	{
		return;
	}

	
	int i=0;
	while (!feof(file)) {
		fscanf(file, "%d, %d, %d, %d,\n", &a, &b, &c, &dummy);
		indices[i].v[0]=a;
		indices[i].v[1]=b;
		indices[i].v[2]=c;
		i++;
	}
	fclose(file);
	m_indexCount = i;
	
	
}
//-----------------------------------------------------------------------------------
void Teapot::LoadNormalsFromFile()
{
	float nx,ny,nz;

	// Load the vertices
	FILE *file = fopen(".\\TeapotNorm.txt", "rt");	
	if (!file) {
		//MessageBox(NULL, "3D data file not found", NULL, MB_ICONASTERISK | MB_OK );
		return;
	}


	int i=0;
	/*while (!feof(file)) {
		fscanf(file, "%f %f %f,\n", &nx,&ny,&nz);
		vertices[i].norm.x=nx;
		vertices[i].norm.y=ny;
		vertices[i].norm.z=nz;
		i++;
	}
		WILL ADD THIS LATER :)
	*/
	fclose(file);
}