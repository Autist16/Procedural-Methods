// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "TerrainShader.h"
#include "DepthShader.h"
#include "ColourShader.h"

#include "cameraControl.h"
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
	void gui();
	void processInput();
	void textureLoading();
	//rendering functions

private:
	//shaders
	DepthShader* depthShader;
	TerrainShader* terrainShader;
	ColourShader* colourShader;

	//render targets
	RenderTexture* depthMap;

	//meshes
	TerrainMesh* terrainMesh;
	Model* skyMesh;
	//positioning matrixes
	XMMATRIX terrainMatrix;
	XMMATRIX skyMatrix;

	//textures
	ID3D11ShaderResourceView* terrainTextures[6];
	PatternCreation* patternGenerator;
	ID3D11ShaderResourceView* skyTextures[2];

	//lighting
	Light* mainLight;

	//control systems & variables
	CameraControl* cameraController;

	//bool wireOn = false;
	//set up variables
	int res = 256;
	float time = 0.0f;
};

#endif