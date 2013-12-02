struct Obj
{
    float3 pos;
	float3 ang;
	int collision;
	float4 color;
	float positionx;
};

//StructuredBuffer<Obj> Buffer0 : register(t0);
RWStructuredBuffer<Obj>  Positions : register(u0);
RWStructuredBuffer<float>  Heights : register(u1);
float imagex = 480.0; //base
float imagey = 480.0; //height

uint rand_xorshift(uint rng_state)
{
    // Xorshift algorithm from George Marsaglia's paper
    rng_state ^= (rng_state << 13);
    rng_state ^= (rng_state >> 17);
    rng_state ^= (rng_state << 5);
    return rng_state;
}

uint wang_hash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

[numthreads(10, 1, 1)]
void main( uint3 dispatchThreadID : SV_DispatchThreadID )
{
	float speed = float(rand_xorshift(wang_hash(dispatchThreadID.x))) * (1.0 / 4294967296.0)+ 0.3;

	//speed = float(rand_xorshift(speed)) * (1.0 / 4294967296.0);
	speed = speed * sin(speed*speed) + 1.0;
	//BufferOut[dispatchThreadID.x].pos = Buffer0[dispatchThreadID.x].pos;
	Obj o = Positions[dispatchThreadID.x];
  	
	//VIEW OF CAMERA  4 a -4 en X  -6 a 6 en Y
	float imageLocationInX =  640.0 *((o.positionx+6.0)/12.0);
	//int imageLocationInX = abs((o.positionx + 6.0) * imagex / 12.0); // Position in x -6 to 6 = 12 units
	float pixel = Heights[imageLocationInX];
	float imageLocationInY = (pixel * 8 / 480) - 4; //position in Y -4 to 4 = 8 units

	//o.color = o.color;
	//o.color = float4((o.positionx+6.0)/12.0, 0,0, 1);
	o.color = float4(pixel/640.0, 0, 0, 1);

	   // INFINITO!
	   if(o.pos.y < imageLocationInY){ // Colission !! 
		   //o.pos.y= 5.0 +  0.1 * dispatchThreadID.x;
		   o.collision = 1;
		   o.ang.y = o.ang.y - 0.06* speed;
		   o.pos.y = o.pos.y - 0.02 * speed;

		   if(o.ang.y < -4){  //Out of bounds in the bottom of the screen 
			   o.collision = 0;
			   o.pos.y = 5.0 + 0.1 * dispatchThreadID.x;
			   o.ang.y = 0.0f;
		   }
	   }else{
			o.pos.y = o.pos.y - 0.06  * speed;
			
	   }
	   o.pos.x = o.pos.x;
	   o.pos.z = dispatchThreadID.x;
	   Positions[dispatchThreadID.x] = o;
    //BufferOut[dispatchThreadID.x].pos.y = 0.0001;s
}