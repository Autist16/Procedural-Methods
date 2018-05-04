// Colour pixel/fragment shader
// Basic fragment shader outputting a colour
Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LayoutBuffer : register(cb0)
{
	float3 pos;
	float radius;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};


float4 main(InputType input) : SV_TARGET
{
	float4 texColour  = shaderTexture.Sample(SampleType, input.tex);
	float4 colour = (0.f,0.f,0.f,0.0f);
	float2 centrePts[2];
	centrePts[0] = float2(pos.x, pos.z);
	centrePts[1] = float2(pos.y, pos.z);
	for (int i = 0; i < 2; i++)
	{
		float XA = input.position.x - centrePts[i].x;
		float YB = input.position.y - centrePts[i].y;
		float r = radius-20;
		if ((XA*XA) + (YB*YB) - (r*r) < 0)
		{
			colour = texColour;
		}
		else if (((XA*XA) + (YB*YB) - (r*r) > 0) && ((XA*XA) + (YB*YB) - (radius*radius) < 0))
		{
			if (colour.x != texColour.x)
			{
				colour = texColour;
				colour.x /= 2;
				colour.y /= 3;
				colour.z /= 2;
			}
		}
	}
	colour -= float4(0.1, 0.1, 0.1, 0.0f);
	colour.a = 1.0f;
	return colour;
}