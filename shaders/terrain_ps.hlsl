
Texture2D shaderTexture[5] : register(t0);
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
	float4 textureColor[5];
	float4 textureToUse[2];
	float4 finalTexture;
	float lightIntensity = 0.0f;

	int nextArea = floor(input.area.y+0.2);
	float blendVal = input.area.y - nextArea;
	//get texture colours
	for (int i = 0; i < 5; i++)
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
	int mainType = floor(input.area.x + 0.5);
	switch (mainType)
	{
		case 0: 
		{
			textureToUse[0] = textureColor[2]; //water
			break; 
		}
		case 1 :
		{
			textureToUse[0] = textureColor[3]; //grass
			break;
		}
		case 2:
		{
			textureToUse[0] = textureColor[0];
			break;
		}
		case 3:
		{
			textureToUse[0] = textureColor[1]; //mountain
			break;
		}
		case 4:
		{
			textureToUse[0] = textureColor[4]; //snow
			break;
		}
	}

	float4 tempTex = textureToUse[0];

	//area blending
	if (mainType == 3 && nextArea != 4)
	{
		if (input.normal.y > 0.65)
		{
			tempTex = lerp(textureToUse[0], textureColor[0], input.normal.y);
		}
	}
	else if (mainType == 0 && input.normal.y <= 0.3)
	{
		tempTex = textureColor[3];
	}
	else
	{
		tempTex = lerp(textureToUse[0], textureColor[0], blendVal);
	}
	
	colour = colour * tempTex;
	colour = saturate(colour);
	return colour;
}