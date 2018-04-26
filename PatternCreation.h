//used to create the pattern used by the sky box
#ifndef _PATTERNCREATION_H_
#define _PATTERNCREATION_H_

#include "../DXFramework/BaseApplication.h"

class PatternCreation
{
public: 
	PatternCreation(int res);
	~PatternCreation();
	void CreateTexture(ID3D11Device* device);
	ID3D11ShaderResourceView* getCloudTexture(ID3D11Device* device);
	ID3D11ShaderResourceView* getDetailTexture(ID3D11Device* device);
	int getNoPatterns()
	{
		return noPatterns;
	}


protected:
	void noise2D();
	void noiseCreation();

	int resolution;
	int noPatterns;
	XMFLOAT4* cloudPattern;
	XMFLOAT4* detailPattern;
	float* textureValue;

	ID3D11Texture2D *cloudTexture;
	ID3D11Texture2D *detailTexture;
	XMFLOAT2* seedValues2;
	float* noiseOutput2D;
	float* detailOutput2D;
};


#endif // ! _PATTERNCREATION_H_


