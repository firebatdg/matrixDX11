//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>

#ifndef _BUFFERS_H_
#define _BUFFERS_H_

struct InstanceType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 angle;
		int collision;
		D3DXVECTOR4 color;
		float positionx;
	};

#endif