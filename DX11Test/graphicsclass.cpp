////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "graphicsclass.h"
#include "Buffers.h"
#include <time.h>
#include <stdlib.h>

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
	count = 0;

	cvNamedWindow( "result", CV_WINDOW_AUTOSIZE );
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	HRESULT result;


	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	
	// Create the model object.
	//m_Model = new ModelClass;

	//m_Model = new Teapot();
	//m_ModelCero = OBJModel(L".\\cero.obj"); // :P 
	m_ModelUno = new OBJModel(L".\\uno.obj", 0.2,0);
	m_Model = new OBJModel(L".\\cero.obj",0.2, 1); // :P Se esta inicializando el cero

	personDetector = new PersonDetector();

	if(!m_Model)
	{
		return false;
	}
	if (!m_ModelUno)
	{
		return false;
	}
	// Initialize the model object.
	result = m_Model->Initialize(m_D3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	result = m_ModelUno->Initialize(m_D3D->GetDevice());
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the color shader object.
	m_ColorShader = new ColorShaderClass;
	if(!m_ColorShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	/* Initialize DX11 Compute Shader */

	ID3D10Blob *csBuffer = 0;
	ID3D10Blob *errorMessage = 0;


	// Compile ComputeSHader
	result = D3DX11CompileFromFile(L".\\physics.hlsl", 0, 0, "main", "cs_5_0", 0, 0, 0, &csBuffer, &errorMessage, 0);
	if(FAILED(result)){
		CheckErrors(result);
		m_ColorShader ->OutputShaderErrorMessage(errorMessage,hwnd, L"Compute Shader" );
	}

	// Initialize ComputeShader
	result = m_D3D->GetDevice()->CreateComputeShader(csBuffer->GetBufferPointer(), csBuffer->GetBufferSize(), NULL, &m_ComputeShader);
	if(FAILED(result)){
		CheckErrors(result);
		int z=3; //debug :)
	}

	    /* -----------------------------------
		 * Heights Buffer 
		 * -----------------------------------*/

		int IMG_SIZE = 640;
		float *data = new float[IMG_SIZE];
		for(int i=0;i<IMG_SIZE;i++){
			data[i]=i;
		}

		D3D11_BUFFER_DESC hDesc;
		// Set up the description of the instance buffer.
		hDesc.Usage = D3D11_USAGE_DEFAULT;
		//pDesc.Usage = D3D11_USAGE_DYNAMIC;
		hDesc.ByteWidth = sizeof(float) * IMG_SIZE;
		hDesc.BindFlags =  D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		hDesc.CPUAccessFlags = 0;
		//pDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		hDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		hDesc.StructureByteStride = sizeof(float);


		D3D11_SUBRESOURCE_DATA hData;
		// Give the subresource structure a pointer to the height data.
		hData.pSysMem = data;
		hData.SysMemPitch = 0;
		hData.SysMemSlicePitch = 0;

		// Create the heights buffer.
		result = m_D3D->GetDevice()->CreateBuffer(&hDesc, &hData, &m_HeightsBuffer);
		if (FAILED(result))
        {
			CheckErrors(result);
           int k=0;
        }

		//SECOND BUFFER
		D3D11_BUFFER_DESC h2Desc;
		h2Desc.Usage = D3D11_USAGE_DYNAMIC;
        h2Desc.ByteWidth = sizeof(float) * IMG_SIZE;
        h2Desc.BindFlags =   D3D11_BIND_VERTEX_BUFFER;// | D3D11_BIND_SHADER_RESOURCE;
        h2Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;// | D3D11_CPU_ACCESS_READ;
        h2Desc.MiscFlags =0;// D3D11_RESOURCE_MISC_SHARED;
        h2Desc.StructureByteStride = sizeof(float);
		
		D3D11_SUBRESOURCE_DATA h2Data;
		h2Data.pSysMem = data;
		h2Data.SysMemPitch = 0;
		h2Data.SysMemSlicePitch = 0;

		// Create the heights buffer.
		result = m_D3D->GetDevice()->CreateBuffer(&h2Desc, &h2Data, &m_tempBuffer);
		if (FAILED(result))
        {
			CheckErrors(result);
           int k=0;
        }

	    /* -----------------------------------
		 * Physics Buffer
		 * -----------------------------------*/
		int numrand = (rand() % 10);
		srand(numrand*time(NULL));// time(NULL));

		int instanceCount = m_Model->GetInstanceCount()*2;

		InstanceType *physicsData = new InstanceType[instanceCount];
		ZeroMemory(physicsData, instanceCount * sizeof(InstanceType));
		for(int i=0;i<instanceCount;i++){
			//memcpy((void*)(physicsData+i), (void*)(m_Model->m_instances+i), sizeof(InstanceType));
			//_memccpy((void*)(physicsData+m_Model->GetInstanceCount() + i), (void*)(m_ModelUno->m_instances+i), NULL, sizeof(InstanceType));

			float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			physicsData[i].position = D3DXVECTOR3(((r * 14.0) - 7.0),(rand() % 60)/10.0f - 3.0f,	i);
			//physicsData[i].position = D3DXVECTOR3((rand() % 12)- 6.0f, (rand() % 60)/10.0f - 3.0f,	i);
			physicsData[i].color = D3DXVECTOR4(0, 0,0, 1.0f);
			physicsData[i].collision = 0;
			physicsData[i].positionx =  physicsData[i].position.x;
			//physicsData[m_Model->GetInstanceCount() + i] = m_ModelUno->m_instances[i];
		}

		D3D11_BUFFER_DESC pDesc;
		// Set up the description of the instance buffer.
		pDesc.Usage = D3D11_USAGE_DEFAULT;
		//pDesc.Usage = D3D11_USAGE_DYNAMIC;
		pDesc.ByteWidth = sizeof(InstanceType) * instanceCount;
		pDesc.BindFlags =  D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		pDesc.CPUAccessFlags = 0;
		//pDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		pDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		pDesc.StructureByteStride = sizeof(InstanceType);


		D3D11_SUBRESOURCE_DATA pData;
		// Give the subresource structure a pointer to the height data.
		pData.pSysMem = physicsData;
		pData.SysMemPitch = 0;
		pData.SysMemSlicePitch = 0;

		// Create the heights buffer.
		result = m_D3D->GetDevice()->CreateBuffer(&pDesc, &pData, &m_PhysicsBuffer);
		  if (FAILED(result))
        {
			CheckErrors(result);
           int k=0;
        }


       D3D11_UNORDERED_ACCESS_VIEW_DESC descView;
       ZeroMemory( &descView, sizeof(descView) );
       descView.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
       descView.Buffer.FirstElement = 0;

       descView.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
	   descView.Buffer.NumElements = instanceCount; //m_Model->GetInstanceCount();
               
	   if(FAILED(m_D3D->GetDevice()->CreateUnorderedAccessView( m_PhysicsBuffer, &descView, &instanceDestView )))
		   CheckErrors(result);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.ViewDimension   	 = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.ElementOffset = 0;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Format					= DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.NumElements = instanceCount;// m_Model->GetInstanceCount();

		result = m_D3D ->GetDevice() -> CreateShaderResourceView( m_PhysicsBuffer , &srvDesc, &srv);

		if(FAILED(result)){
			CheckErrors(result);
			int yx=3; //debug :)
		}
	

		D3D11_UNORDERED_ACCESS_VIEW_DESC descHeightView;
		ZeroMemory(&descHeightView, sizeof(descHeightView));
		descHeightView.ViewDimension	= D3D11_UAV_DIMENSION_BUFFER;
		//descHeightView.ViewDimension = D3D11_UAV_DIMENSION_UNKNOWN;
		descHeightView.Buffer.FirstElement = 0;
		descHeightView.Format = DXGI_FORMAT_UNKNOWN;
		descHeightView.Buffer.NumElements = IMG_SIZE; // Real buffer size;

		result = m_D3D->GetDevice()->CreateUnorderedAccessView( m_HeightsBuffer, &descHeightView, &heightsView );
		if(FAILED(result)){
			CheckErrors(result);
			int yx=3; //debug :)
		}
          



	return true;
}

void GraphicsClass::Shutdown()
{
	// Release the color shader object.
	if(m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// Release the model object.
	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}
	if (m_ModelUno)
	{
		m_ModelUno->Shutdown();
		delete m_ModelUno;
		m_ModelUno = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}

bool GraphicsClass::Frame()
{
	bool result;
	// Render the graphics scene.
	result = Render();
	if(!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(1.0f, 1.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	count++;
	if(count > 5){

		personDetector->captureFrame();
		vector<float> dataVector = personDetector->getHumans();
		float *data = &dataVector[0];
		/*float *data = new float[640];
		for(int i=0;i<640;i++){
			data[i] = (float)i;
		}
		*/
		Mat demo(personDetector->final.rows, personDetector->final.cols, CV_8UC1);
		demo = Scalar(0,0,0);
		for(int i=0;i<dataVector.size(); i++){
			demo.at<uchar>(dataVector.at(i),i) = 0xFF;
		}

		auto m_d3dContext =m_D3D->GetDeviceContext();
		HRESULT res = m_d3dContext->Map(m_tempBuffer, NULL, D3D11_MAP_WRITE_DISCARD , NULL, &mappedResource);
		//int x=2;
		memcpy(mappedResource.pData, data, sizeof(float)*dataVector.size());
		m_d3dContext->Unmap(m_tempBuffer, 0);

		m_d3dContext->CopyResource(m_HeightsBuffer ,m_tempBuffer);

		count = 0;
		imshow( "result", demo );
		imshow( "original", personDetector->final);
		//waitKey( 10 );
	}

	
	// COMPUTE SHADER!
	

	m_D3D->GetDeviceContext()->CSSetShader(m_ComputeShader, 0, 0);
	m_D3D->GetDeviceContext()->CSSetShaderResources(0, 1, &srv);
	m_D3D->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &instanceDestView, NULL); // Instance Buffers
	m_D3D->GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, &heightsView, NULL); // Height Buffer
	m_D3D->GetDeviceContext()->Dispatch((m_Model->GetInstanceCount()*2)/10, 1, 1);

	ID3D11UnorderedAccessView* pNullUAV = NULL;
	ID3D11ShaderResourceView* pSrvNull[2] = {NULL, NULL};
	m_D3D->GetDeviceContext()->CSSetShader(NULL, NULL, 0);
	m_D3D->GetDeviceContext()->CSSetUnorderedAccessViews( 0, 1, &pNullUAV, NULL );
	m_D3D->GetDeviceContext()->CSSetShaderResources(0, 2, pSrvNull);

	//m_D3D->GetDeviceContext()->VSSetShaderResources(0, 1, &srv);
	m_ColorShader -> m_srv = srv;


	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_D3D->GetDeviceContext());

	// Render the model using the color shader.
	result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Model->GetVertexCount(), m_Model ->GetInstanceCount(),  m_Model -> GetIndexCount() * 3, worldMatrix, viewMatrix, projectionMatrix);
	if(!result)
	{
		return false;
	}

	//--------------------------------------------EL UNO
	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_ModelUno->Render(m_D3D->GetDeviceContext());

	// Render the model using the color shader.
	result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_ModelUno->GetVertexCount(), m_ModelUno->GetInstanceCount(), m_ModelUno->GetIndexCount() * 3, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}
	//------------------------------------------------------

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

void GraphicsClass::CheckErrors(HRESULT result){
	//switch (HRESULT_CODE(result)){
	switch (result){
		case D3D11_ERROR_FILE_NOT_FOUND:
			MessageBox(NULL,L"The file was not found.",L"D3D11_ERROR_FILE_NOT_FOUND",MB_ICONERROR);
			break;
		case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
			MessageBox(NULL,L"There are too many unique instances of a particular type of state object.",L"D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS",MB_ICONERROR);
			break;
		case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
			MessageBox(NULL,L"There are too many unique instances of a particular type of view object.",L"D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS",MB_ICONERROR);
			break;
		case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD:
			MessageBox(NULL,L"The first call to ID3D11DeviceContext::Map after either ID3D11Device::CreateDeferredContext or ID3D11DeviceContext::FinishCommandList per Resource was not D3D11_MAP_WRITE_DISCARD.",L"D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD",MB_ICONERROR);
			break;
		case D3DERR_INVALIDCALL:
			MessageBox(NULL,L"The method call is invalid. For example, a method's parameter may not be a valid pointer.",L"D3DERR_INVALIDCALL",MB_ICONERROR);
			break;
		case DXGI_ERROR_INVALID_CALL:
			MessageBox(NULL,L"The method call is invalid. For example, a method's parameter may not be a valid pointer.",L"DXGI_ERROR_INVALID_CALL",MB_ICONERROR);
			break;
		case D3DERR_WASSTILLDRAWING:
			MessageBox(NULL,L"The previous blit operation that is transferring information to or from this surface is incomplete.",L"D3DERR_WASSTILLDRAWING",MB_ICONERROR);
			break;
		case DXGI_ERROR_WAS_STILL_DRAWING:
			MessageBox(NULL,L"The previous blit operation that is transferring information to or from this surface is incomplete.",L"DXGI_ERROR_WAS_STILL_DRAWING",MB_ICONERROR);
			break;
		case E_FAIL:
			MessageBox(NULL,L"Attempted to create a device with the debug layer enabled and the layer is not installed.",L"E_FAIL",MB_ICONERROR);
			break;
		case E_INVALIDARG:
			MessageBox(NULL,L"An invalid parameter was passed to the returning function.",L"E_INVALIDARG",MB_ICONERROR);
			break;
		case E_OUTOFMEMORY:
			MessageBox(NULL,L"Direct3D could not allocate sufficient memory to complete the call.",L"E_OUTOFMEMORY",MB_ICONERROR);
			break;
		case S_FALSE:
			MessageBox(NULL,L"Alternate success value, indicating a successful but nonstandard completion (the precise meaning depends on context).",L"S_FALSE",MB_ICONERROR);
			break;
		case S_OK:
			MessageBox(NULL,L"No error occurred.",L"S_OK",MB_ICONERROR);
			break;
		default:
			MessageBox(NULL,L"DEFAULT CASE no error catched",L"DEFAULT CASE",MB_ICONERROR);
			break;
	}
}
