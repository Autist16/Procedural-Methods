// Colour pixel/fragment shader
// Basic fragment shader outputting a colour
Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
	float4 texColour = shaderTexture.Sample(SampleType, input.tex);
	float4 colour = float4(0.3f, 0.4, 0.85, 1.0);
	colour += texColour;
	colour = saturate(colour);
	return colour;
}