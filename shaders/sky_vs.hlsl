// colour vertex shader
// Simple geometry pass
// texture coordinates and normals will be ignored.

cbuffer MatrixBuffer : register(cb0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};
cbuffer TimeBuffer : register(cb1)
{
	float time;
	float3 timePadding;

};

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float2 tex2 : TEXCOORD1;
};

OutputType main(InputType input)
{
	OutputType output;
	
	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;


	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = float2(input.tex.x*3, input.tex.y*3);
	output.tex2.x = input.tex.x*6 - time/20;
	output.tex2.y = input.tex.y*6 - time/20;
	output.tex.x -= time/40;
	// Store normals for the pixel shader
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	return output;
}