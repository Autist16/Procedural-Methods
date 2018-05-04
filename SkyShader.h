// Colour shader.h
// Simple shader example.
#ifndef _SKYSHADER_H_
#define _SKYSHADER_H_

#include "../DXFramework/BaseShader.h"

using namespace std;
using namespace DirectX;


class SkyShader : public BaseShader
{
	struct timeBufferType
	{
		float time;
		XMFLOAT3 timePadding;
	};
public:

	SkyShader(ID3D11Device* device, HWND hwnd);
	~SkyShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture[], float time);
	void render(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	void initShader(WCHAR*, WCHAR*);
	void createRasterState(ID3D11Device* device);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11RasterizerState* rasterState;
};

#endif