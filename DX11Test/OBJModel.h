#pragma once
using namespace std;

#include "modelclass.h"
#include <fstream>
#include <istream>
#include <vector>


//Herencia de model class  
class OBJModel : 
	public ModelClass
{
private:
	//variables

	//Direccion del archivo a leer
	wstring m_strModelPath;

	IndexType* m_indices;

	//contenedor de vertices un puntero de un tipo de dato 
	//funciona como un arreglo del mismo tipo
	VertexType* m_vertexes;
	

	float m_fScale;

	//Fuctions
public:

	InstanceType* m_instances;
	//Constructor con asignación al objeto a leer
	OBJModel(wstring modelPath, float scale, int index);
	~OBJModel(void);

	virtual bool InitializeBuffers(ID3D11Device* d3dDev);
	virtual void RenderBuffers(ID3D11DeviceContext* d3dDevContext);

private:
	// Lee la informacion del archivo .OBJ que se le ha de asignar
	void LoadVertexesFromOBJ(float scale);

	// Lee los vertices del archivo .OBJ que se le ha asignado
	void LoadIndexesFromOBJ();

	// Lee las normales del archivo .OBJ que se le ha asignado
	void LoadNormalsFromOBJ();

	void LoadUVsFromOBJ();

	int index;
};