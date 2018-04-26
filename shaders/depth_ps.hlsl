struct InputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
	float depthValue1;
	float4 color;

	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
	depthValue1 = input.depthPosition.z / input.depthPosition.w;

	color = float4(depthValue1, 0.0f, 0.0f, 1.0f);
	return color;
}