// Colour shader.h
// Simple shader example.
#ifndef _POSTSHADER_H_
#define _POSTSHADER_H_

#include "../DXFramework/BaseShader.h"

using namespace std;
using namespace DirectX;


class PostProcessingShader : public BaseShader
{
	struct layoutBufferType
	{
		XMFLOAT3 pts;
		float radius;
	};

public:

	PostProcessingShader(ID3D11Device* device, HWND hwnd);
	~PostProcessingShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, int width, int height);
	void render(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	void initShader(WCHAR*, WCHAR*);


private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* layoutBuffer;
	ID3D11SamplerState* sampleState;
};

#endif