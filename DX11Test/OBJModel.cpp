#include "stdafx.h"
#include "OBJModel.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>

//Constructor con asignación al objeto a leer

OBJModel::OBJModel(wstring modelPath, float scale, int ind)
{
	m_strModelPath = modelPath;
	m_fScale = scale;
	m_vertexes = new VertexType[1500];
	m_indices = new IndexType[3000];
	index = ind;
}

OBJModel::~OBJModel()
{

}


bool OBJModel::InitializeBuffers(ID3D11Device* d3dDev)
{
	this->LoadVertexesFromOBJ(m_fScale);
	this->LoadIndexesFromOBJ();
	this->LoadNormalsFromOBJ();
	this->LoadUVsFromOBJ();

	//en su debido momento tambien podras agregar las normales aqui :D 

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
	vertexData.pSysMem = m_vertexes;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;


	// Now create the vertex buffer.
	result = d3dDev->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}


	// Set the number of instances in the array.
	m_instanceCount = 360;


	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] m_vertexes;
	m_vertexes = 0;

	// INSTANCING

	// Create the instance array.
	m_instances = new InstanceType[m_instanceCount];
	if (!m_instances)
	{
		return false;
	}
	ZeroMemory(m_instances, m_instanceCount * sizeof(InstanceType));

	// Load the instance array with data.
	int numrand = (rand() % 10);
	srand(numrand*time(NULL));// time(NULL));
	for (int i = 0; i < m_instanceCount ; i++){
		m_instances[i].position = D3DXVECTOR3((rand() % 12)- 6.0f, (rand() % 60)/10.0f - 3.0f,	m_instanceCount*index + i);
		m_instances[i].color = D3DXVECTOR4(0, 0, m_instances[i].position.x, 1.0f);
		m_instances[i].collision = 0;
		m_instances[i].positionx =  m_instances[i].position.x;
		
	}
	
	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags =  D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	instanceBufferDesc.StructureByteStride = sizeof(InstanceType);


	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = m_instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = d3dDev->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if (FAILED(result))
	{
		return false;
	}
	                                                                                                                       
	// Release the instance array now that the instance buffer has been created and loaded.
	delete [] m_instances;
	m_instances = 0;



	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(IndexType) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;


	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = m_indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = d3dDev->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}
	delete [] m_indices;
	m_indices = 0;

	return true;
}

void OBJModel::RenderBuffers(ID3D11DeviceContext* d3dDevContext)
{
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
	d3dDevContext->IASetVertexBuffers(0, 3, bufferPointers, strides, offsets);


	// Set the index buffer to active in the input assembler so it can be rendered.
	d3dDevContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);


	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//POINTLIST , LINELIST , TRIANGLELIST
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

// Lee la informacion del archivo obj que se le ha de asignar
void OBJModel::LoadVertexesFromOBJ(float scale)
{
	//Copiamos el objeto para poder utilizarlo de nuevo luego sin tener que hacer la lectura una y otra vez
	wifstream auxStream = wifstream(m_strModelPath.c_str());
	wchar_t currentCharacter;

	int counter=0;

	if (auxStream.is_open())
	{
		while (!auxStream.eof())
		{
			currentCharacter = auxStream.get();

			switch (currentCharacter)
			{
			case 'v':
				{
					//Nos comemos un espacio porque siempre hay uno despues de v para llegar a posiciones
					currentCharacter = auxStream.get();
					if (currentCharacter == ' ')
					{
						float pos_x;
						float pos_y;
						float pos_z;

						// '<<' es escritura  y '>>' es lectura, 
						// ahora leemos tres flotantes cuando le decimos lo siguiente
						/*de donde   float_1  float_2  float_3
						leemos     leido    leido    leido     */
						auxStream >> pos_x >> pos_y >> pos_z;

						m_vertexes[counter].position.x = pos_x*scale;
						m_vertexes[counter].position.y = pos_y*scale;
						m_vertexes[counter].position.z = pos_z*scale;

						m_vertexes[counter].color = D3DXVECTOR4(0.5, 0.02*counter+0.1, 1.0, 0.5);
						counter++;
					}
					else
					{
						break;
					}
				}
				break;
			default:
				break;
			}
		}
	}

	m_vertexCount = counter;
	auxStream.close();
}


// Lee los vertices del archivo .OBJ que se le ha asignado
void OBJModel::LoadIndexesFromOBJ()
{
	//Copiamos el objeto para poder utilizarlo de nuevo luego sin tener que hacer la lectura una y otra vez
	wifstream auxStream = wifstream(m_strModelPath.c_str());
	wchar_t currentCharacter;

	vector<_D3DVECTOR> vertexPositions;
	int counter = 0;
	wstring auxStrIndex;

	if (auxStream.is_open())
	{
		while (!auxStream.eof())
		{
			currentCharacter = auxStream.get();

			switch (currentCharacter)
			{
			case 'f':
				{
					currentCharacter = auxStream.get();
					if (currentCharacter == ' ')
					{
						//Guardar los indices de f
						vector<wstring> str = vector<wstring>();

						while (currentCharacter != '\n'){

							if (currentCharacter == ' '){//______________inicia dato
								currentCharacter = auxStream.get();
								while (currentCharacter != '/'){
									auxStrIndex += currentCharacter;
									currentCharacter = auxStream.get();
								}
							}

							if (currentCharacter == '/' && auxStrIndex.length()!=0){	//______________termina dato
								str.push_back(auxStrIndex);
								auxStrIndex.clear();
							}

							currentCharacter = auxStream.get();
						}

						//Si hay mas de 4 caras muere
						if (str.size() > 4)
						{
							wstring out = L"";
							out = str[0].append(L" ").append(str[1]).append(L" ").append(str[2]).append(L" ").append(str[3]);
							MessageBox(NULL, out.c_str()  , L"HI", MB_ICONEXCLAMATION);
							if (MessageBox(NULL, L"El modelo tiene mas de 4 lados por cara, revisa tu obj", L"Model Error", MB_ICONERROR))
								
								ExitProcess(-1);
						}

						// Agregar los indices
						m_indices[counter].v[0] = (int) _wtoi(str[0].c_str())-1;
						m_indices[counter].v[1] = (int) _wtoi(str[1].c_str())-1;
						m_indices[counter].v[2] = (int) _wtoi(str[2].c_str())-1;
						counter++;
						if (str.size() == 4)
						{
							m_indices[counter].v[0] = (int) _wtoi(str[0].c_str())-1;
							m_indices[counter].v[1] = (int) _wtoi(str[2].c_str())-1;
							m_indices[counter].v[2] = (int) _wtoi(str[3].c_str())-1;
							counter++;
						}
					}
				}
				break;

			default:
				break;
			}
		}
	}
	m_indexCount = counter;
	auxStream.close();
}

void OBJModel::LoadUVsFromOBJ()
{
	float u,v;

	// Load the vertices
	 const wchar_t* path = m_strModelPath.c_str();
	  ifstream File(path);

	int i=0;
	int n=0;
	string line;
	while (getline(File, line)) {
		

		n = sscanf(line.c_str(), "vt %f %f\n", &u,&v);
		if(n != 2){
			
			continue;
		}
		m_vertexes[i].UV = D3DXVECTOR2(u, v);
		i++;

	}
	
	File.close();

}

// Lee las normales del archivo .OBJ que se le ha asignado
void OBJModel::LoadNormalsFromOBJ()
{


	float nx,ny,nz;

	// Load the vertices
	 const wchar_t* path = m_strModelPath.c_str();
	  ifstream File(path);

	int i=0;
	int n=0;
	string line;
	while (getline(File, line)) {
		

		n = sscanf(line.c_str(), "vn %f %f %f\n", &nx,&ny,&nz);
		if(n != 3){
			
			continue;
		}
		m_vertexes[i].normal = D3DXVECTOR3(nx, ny, nz);
		i++;

	}
	
	File.close();

}
