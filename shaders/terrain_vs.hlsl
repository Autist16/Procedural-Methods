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
 }

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float2 area : TEXCOORD1;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float2 area : TEXCOORD1;
};

OutputType main(InputType input)
{
	OutputType output;
	float4 worldPosition = mul(input.position, worldMatrix);
	float4 vertexPos = input.position;
	float3 vertexNorm = input.normal;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;
	float areaVal2 = input.area.y - (int)input.area.y;
	if (input.area.x == 3.0f)
	{
		vertexPos.y += 0.2*cos(vertexPos.z*(4 * vertexPos.x)+ time) + 0.1*sin(vertexPos.x*vertexPos.z+time);
		vertexNorm.z += -abs(sin(vertexPos.z*(4 * vertexPos.x) + time) + cos(vertexPos.x * vertexPos.z + time));
		vertexNorm.x += 1 - (sin(vertexPos.z*(4 * vertexPos.x) + time) + cos(vertexPos.x * vertexPos.z + time));
	}
	
	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(vertexPos, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.area = input.area;

	
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Store normals for the pixel shader
	output.normal = mul(vertexNorm, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	return output;
}