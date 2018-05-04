#include "PatternCreation.h"

PatternCreation::PatternCreation(int res)
{
	cloudTexture = nullptr;
	detailTexture = nullptr;
	
	resolution = res;
	noPatterns = 2;
	seedValues2 = new XMFLOAT2[res*res];
	//populate seed array values
	for (int i = 0; i < (res*res); i++)
	{
		float v = (rand() % 10)-4;
		seedValues2[i].x = (float)v/10;// seed values for pattern 1
		float w = (rand() %14)-4;
		seedValues2[i].y = w/ 10; // seed values for pattern 2
	}

	//create storage for texture
	cloudPattern = new XMFLOAT4[res*res];
	detailPattern = new XMFLOAT4[res*res];
	//run noise creation functions
	noiseCreation();
}

PatternCreation::~PatternCreation()
{
	if (cloudTexture)
	{
		cloudTexture->Release();
		cloudTexture = 0;
	}

	if (detailTexture)
	{
		detailTexture->Release();
		detailTexture = 0;
	}
}
void PatternCreation::CreateTexture(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC cloudTextureDesc; // this desc can be reused for both texturess
	D3D11_SUBRESOURCE_DATA cloudTextureData, detailTextureData;

	//pass data to texture creator
	cloudTextureData.pSysMem = (XMFLOAT4*)cloudPattern;
	cloudTextureData.SysMemPitch = resolution * 4 * sizeof(float);
	cloudTextureData.SysMemSlicePitch = 0;

	detailTextureData.pSysMem = (XMFLOAT4*)detailPattern;
	detailTextureData.SysMemPitch = resolution * 4 * sizeof(float);
	detailTextureData.SysMemSlicePitch = 0;

	//create texture2d desc
	cloudTextureDesc.Width = resolution;			// width = height to match noise generation
	cloudTextureDesc.Height = resolution;			// to prevent stretching or warpping at this inital stage
	cloudTextureDesc.MipLevels = 1;
	cloudTextureDesc.ArraySize = 1;
	cloudTextureDesc.SampleDesc.Count = 1;
	cloudTextureDesc.SampleDesc.Quality = 0;
	cloudTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	cloudTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	cloudTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	cloudTextureDesc.CPUAccessFlags = 0;
	cloudTextureDesc.MiscFlags = 0;
	
	//create texture	reuse desc for both textures as same set ups
	device->CreateTexture2D(&cloudTextureDesc, &cloudTextureData, &cloudTexture);
	device->CreateTexture2D(&cloudTextureDesc, &detailTextureData, &detailTexture);
}
ID3D11ShaderResourceView* PatternCreation::getCloudTexture(ID3D11Device* device)
{
	ID3D11ShaderResourceView* textureOut;
	device->CreateShaderResourceView(cloudTexture,NULL, &textureOut);

	return textureOut;
}
ID3D11ShaderResourceView* PatternCreation::getDetailTexture(ID3D11Device* device)
{
	ID3D11ShaderResourceView* textureOut;
	device->CreateShaderResourceView(detailTexture, NULL, &textureOut);

	return textureOut;
}
 
//get and apply noise to patterns
void PatternCreation::noiseCreation()
{
	int index = 0;
	int mVal= resolution;
	float* colourVal = new float[noPatterns];

	noise2D();
	
	//apply noise to array
	int noiseIndex = 0;

	for (int i = 0; i < resolution; i++)
	{
		for (int j = 0; j < resolution; j++)
		{
			noiseIndex = (i*resolution + j);
			colourVal[0] = noiseOutput2D[noiseIndex]*1.5;
			colourVal[1] = detailOutput2D[noiseIndex];
			//lock values in valid range for colour in shaders
			for (int i = 0; i < 2; i++)
			{
				
				if (colourVal[i] < 0.0f)
				{
					colourVal[i] = 0.0f;
				}
				else if (colourVal[i] > 1.0f)
				{
					colourVal[i] = 1.0f;
				}
			}
			
			//apply to patterns for textures
			cloudPattern[index] = XMFLOAT4(colourVal[0],colourVal[0],colourVal[0],1.0f);
			detailPattern[index] = XMFLOAT4(colourVal[1], colourVal[1], colourVal[1]+0.05f, 1.0f);
			index++;
		}
	}
}


//generate noise - "perlinLIKE" (definitely NOT perlin)
void PatternCreation::noise2D()
{
	
	int nOutputSize = resolution*resolution;
	noiseOutput2D = new float[nOutputSize];
	detailOutput2D = new float[nOutputSize];
	int width = resolution;
	int height = resolution;

	int noOctaves = 8; // for now
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float noiseVal = 0.0f;
			float detailVal = 0.0f;
			float scalingVal = 1.0f;
			float scaleAcu = 0.0f; //used to check values are valid

			for (int oc = 0; oc < noOctaves; oc++) //for each ocatave
			{
				int nPitch = height >> oc;
				int nSampleX1 = (x / nPitch)*nPitch;
				int nSampleY1 = (y / nPitch)*nPitch;
				
				//wrap round and tesselate
				int nSampleX2 = (nSampleX1 + nPitch) % width; 
				int nSampleY2 = (nSampleY1 + nPitch) % height;

				float BlendX = (float(x - nSampleX1) / (float)nPitch); //how far into pitch
				float BlendY = (float(y - nSampleY1) / (float)nPitch);

				//interpolate between values
				float fSampleT = (1.0f - BlendX)*seedValues2[nSampleY1*width + nSampleX1].x + BlendX*seedValues2[nSampleY1*width + nSampleX2].x;
				float fSampleB = (1.0f - BlendX)*seedValues2[nSampleY2*width + nSampleX1].x + BlendX*seedValues2[nSampleY2*width + nSampleX2].x;

				float dSampleT = (1.0f - BlendX)*seedValues2[nSampleY1*width + nSampleX1].y + BlendX*seedValues2[nSampleY1*width + nSampleX2].y;
				float dSampleB = (1.0f - BlendX)*seedValues2[nSampleY2*width + nSampleX1].y + BlendX*seedValues2[nSampleY2*width + nSampleX2].y;


				//update values
				noiseVal += (BlendY *(fSampleB - fSampleT) + fSampleT)* scalingVal;
				detailVal += (BlendY *(dSampleB - dSampleT) + dSampleT)* scalingVal;
				scaleAcu += scalingVal;
				scalingVal /= 2;
			}
			//scale and store result
			//int i = y*width*x;
			noiseOutput2D[y * width + x] = noiseVal/scaleAcu;
			detailOutput2D[y*width + x] = detailVal/scaleAcu;
		}
	}
}