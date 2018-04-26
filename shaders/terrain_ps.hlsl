
Texture2D shaderTexture[6] : register(t0);
Texture2D depthMapTexture : register(t2);

SamplerState SampleType : register(s0);
SamplerState SampleTypeClamp : register(s1);

cbuffer LightBuffer : register(cb0)
{
	float4 ambient;
	float4 diffuse;
	float3 direction;
	float padding;
}

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float2 area : TEXCOORD1;
};


float4 main(InputType input) : SV_TARGET
{
	float bias = 0.0001f;
	float4 colour = ambient;

	float3 lightDir;
	float4 textureColor[6];
	float4 textureToUse[2];
	float4 finalTexture;
	float lightIntensity = 0.0f;

	int nextArea = (int)input.area.y;
	float blendVal = input.area.y - nextArea;
	//get texture colours
	for (int i = 0; i < 6; i++)
	{
		textureColor[i] = shaderTexture[i].Sample(SampleType, input.tex);
	}

	//flip light direction for calculation
	lightDir = -direction;

	lightIntensity = saturate(dot(input.normal, lightDir));
	if (lightIntensity > 0.0f)
	{
		// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
		colour += saturate(diffuse * lightIntensity);
	}
	else
	{
		lightIntensity = saturate(dot(input.normal, lightDir));
		if (lightIntensity > 0.0f)
		{
			colour += (diffuse*lightIntensity);
			colour = saturate(colour);
		}
	}

	//check if within a particular area
	int mainType = (int)input.area.x;
	switch (mainType)
	{
		case 1 :
		{
			textureToUse[0] = textureColor[0];
			break;
		}
		case 2:
		{
			textureToUse[0] = textureColor[1];
			break;
		}
		case 3:
		{
			textureToUse[0] = textureColor[2];
			break;
		}
		default:
		{
			textureToUse[0] = lerp(textureColor[0],textureColor[3],0.5);
			break;
		}
	}
	
	switch (nextArea)
	{
		case 1:
		{
			textureToUse[1] = textureColor[0];
			break;
		}
		case 2:
		{
			textureToUse[1] = textureColor[1];
			break;
		}
		case 3:
		{
			textureToUse[1] = textureColor[2];
			break;
		}
		default:
		{
			textureToUse[1] = textureColor[3];
			break;
		}
	}
	float4 tempTex= lerp(textureToUse[0], textureToUse[1], blendVal);
	

	//fine detail
	if (mainType == 0)
	{
		if (input.normal.y < 0.3)
		{
			finalTexture = textureColor[4];
		}
		else
		{
			finalTexture = lerp(tempTex, textureColor[4], 1 - input.normal.y);
		}
	}
	else if (mainType == 3 && nextArea != 3)
	{
		if (input.normal.y < 0.15)
		{
			finalTexture = textureColor[4];
		}
		else
		{
			finalTexture = tempTex;
		}
	}
	else if (mainType == 2)
	{
		finalTexture = lerp(tempTex, textureColor[5], 1- input.normal.y);
	}
	else
	{
		finalTexture = lerp(tempTex, textureColor[4], 1 - input.normal.y);
	}
	
	colour = colour * finalTexture;
	colour = saturate(colour);
	return colour;
}