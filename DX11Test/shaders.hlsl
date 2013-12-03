

/////////////
// GLOBALS //
/////////////
#define NUM_LIGHTS 3

									 //float4(1, 1, 1, 1)
                           		

static float3 g_vLightPos[NUM_LIGHTS] = { float3(0, -4, 3),//x,lados , y arriba, z prof
									float3(2, 3, 3),
									float3(-2, -4, 3)
									};
//static float4 g_vLightColor[NUM_LIGHTS] = { float4(0.7, 0.7, 0.7, 1),
//									 float4(0.8, 0, 0, 1),
//									float4(0, 0.8, 0.3, 1)
//									};
static float4 g_vLightColor[NUM_LIGHTS] = { float4(0.1, 0.25, 0.61, 1),
									float4(0.55, 0.8, 0.98, 1),
									float4(0.39, 0.57, 0.97, 1)
};
static float4 g_Ambient    = float4(0.1, 0.1, 0.1, 1);     // Material's ambient color
//static float4 g_Diffuse    = float4(0.2, 0.2, 0.2, 1);    // Material's diffuse color
static float4 g_Diffuse    = float4(0.29, 0.47, 0.87, 1);
static float4 g_Specular   = float4(0.65, 0.65, 0.2, 1);
static float  g_kDiffuse   = 1.0f;
static float  g_PhongExp   = 50.0f;
static float3   g_vEyePos = float3(2,0,2);


cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};


struct VS_INPUT{
	 uint vertexID : SV_VertexID;
	 float4 position : POSITION;
	 float4 color : COLOR;
	 float3 normal : NORMAL;
	 float3 instancePosition : TEXCOORD1;
};

struct VOut
{
    float4 position : SV_POSITION;
	float4 v3DPos : POSITION1;
    float4 color : COLOR;
	float3 normal : NORMAL;
 
};

struct Obj
{
    float3 pos;
	float3 ang;
	int collision;
	float4 color;
	float positionx;
};


float4 PhongModel(float4 Pos, float3 Normal)
{
float4 Color = g_Ambient;
float3 EyeToPos = normalize(Pos.xyz - g_vEyePos);
float3 LightToPos = float3(0,0,0), Reflected = float3(0,0,0);
float4 LightPos = float4(2,20,10, 1);
float DiffuseIntensity = 0, Spec = 0;
for (int i = 0; i < 3; i++)
{
	LightPos = float4(g_vLightPos[i], 1);
	LightToPos = normalize(Pos - LightPos).xyz;
	// Compute the diffuse component
	DiffuseIntensity = saturate(dot(-LightToPos, Normal));
	// Compute the specular component
	Reflected = normalize(LightToPos - 2*dot(Normal, LightToPos) * Normal);
	Spec = saturate(dot(-Reflected, EyeToPos));
	Spec = pow(max(Spec, 0), g_PhongExp);
	Color = Color + g_vLightColor[i] * ((DiffuseIntensity * g_Diffuse) + (Spec * g_Specular));
}
return Color;
}



StructuredBuffer<Obj> Buffer0 : register(t0);

	VOut VShader(float3 position : POSITION, float4 color : COLOR, float3 normal: NORMAL, float3 instancePosition : TEXCOORD1)
{
    VOut output;

	//position.w = 1.0f;
	// Update the position of the vertices based on the data for this particular instance.

	
	
	Obj buffData = Buffer0[instancePosition.z];
	//position.x += instancePosition.x;
    //position.y += buffData.pos.y;

	float3 tmp = float3(buffData.positionx, buffData.pos.y, 1);
	//float3 tmp = float3(7, 0, 1);


	float ang = buffData.ang.y % 360;
	ang = ang * ((instancePosition.z % 2)*2 -1 );

	float3x3 yaw = { cos(ang), -1*sin(ang), 0,
		             sin(ang), cos(ang), 0,
					 0, 0, 1};

	//position = mul(float4(position, 1.0f), worldMatrix).xyz;
	position = mul(position, yaw);
    position.x += tmp.x;
	position.y += tmp.y;

	float4 vPos = float4(position, 1.0f);
    output.v3DPos = mul(vPos, worldMatrix);
    output.normal= normalize( mul( float4(normal, 0), worldMatrix ) ).xyz;
	//output.color = color;
	//output.color = buffData.color;

	   // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(vPos, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    return output;
}


float4 PShader(VOut In) : SV_TARGET
{
	//return In.color;
    return PhongModel(In.v3DPos, In.normal)+g_Diffuse*1.5;
}