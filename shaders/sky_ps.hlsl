// Colour pixel/fragment shader
// Basic fragment shader outputting a colour
Texture2D shaderTexture[2] : register(t0);
SamplerState SampleType : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float2 tex2 : TEXCOORD1;
};


float4 main(InputType input) : SV_TARGET
{
	float4 texColour[2];
	texColour[0]= shaderTexture[0].Sample(SampleType, input.tex);
	texColour[1] = shaderTexture[1].Sample(SampleType, input.tex2);
	float4 colour = float4(0.3f, 0.4, 0.85, 1.0);

	colour += lerp(texColour[0], texColour[1], 0.3);

	colour = saturate(colour);
	return colour;
}