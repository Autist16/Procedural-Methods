// Colour shader.h
// Simple shader example.
#ifndef _TERRAINSHADER_H_
#define _TERRAINSHADER_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Light.h"

using namespace std;
using namespace DirectX;


class TerrainShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};
	struct LightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding;
	};

	struct TimeBufferType
	{
		float time;
		XMFLOAT3 timePadding;
	};
public:
	TerrainShader(ID3D11Device* device, HWND hwnd);
	~TerrainShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, Light *mainLight, ID3D11ShaderResourceView* texture[], float timeP);
	void render(ID3D11DeviceContext* deviceContext, int indexCount);
private:
	void initShader(WCHAR*, WCHAR*);
	void loadVertexShader(WCHAR* filename);
	

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* pixelLightBuffer;
	ID3D11Buffer* timeBuffer;

	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateClamp;
};

#endif