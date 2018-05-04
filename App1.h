// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "TerrainShader.h"
#include "SkyShader.h"
#include "PostProcessing.h"

#include "TerrainMesh.h"
#include "../DXFramework/Model.h"

#include "PatternCreation.h"
class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in);

	bool frame();

protected:
	bool render();
	void renderToTexture();
	void renderScene();
	void gui();
	void processInput();
	void textureLoading();
private:
	//shaders
	TerrainShader* terrainShader;
	SkyShader* skyShader;
	PostProcessingShader* postShader;
	//render targets
	RenderTexture* postProcessTex;

	//meshes
	TerrainMesh* terrainMesh;
	Model* skyMesh;
	OrthoMesh* orthoMesh;

	//positioning matrixes
	XMMATRIX terrainMatrix;
	XMMATRIX skyMatrix;

	//textures
	ID3D11ShaderResourceView* terrainTextures[5];
	PatternCreation* patternGenerator;
	ID3D11ShaderResourceView* skyTextures[2];

	//lighting
	Light* mainLight;

	
	//set up variables
	int res = 256;
	float time = 0.0f;
	XMFLOAT2 camBounds;
	//post processing variables
	bool* zoom;
	bool* wire;
	float scnHeight, scnWidth;
};

#endif