// texture shader.cpp
#include "PostProcessing.h"


PostProcessingShader::PostProcessingShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"post_vs.cso", L"post_ps.cso");
}


PostProcessingShader::~PostProcessingShader()
{
	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void PostProcessingShader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC layoutBuffereDesc;
	D3D11_SAMPLER_DESC samplerDesc;


	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	layoutBuffereDesc.Usage = D3D11_USAGE_DYNAMIC;
	layoutBuffereDesc.ByteWidth = sizeof(layoutBufferType);
	layoutBuffereDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	layoutBuffereDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	layoutBuffereDesc.MiscFlags = 0;
	layoutBuffereDesc.StructureByteStride = 0;
	//create buffer
	renderer->CreateBuffer(&layoutBuffereDesc, NULL, &layoutBuffer);

	//Sampler descriptions
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

}


void PostProcessingShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, int width, int height)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	layoutBufferType* layoutPtr;
	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	// Unlock the constant buffer.
	deviceContext->Unmap(matrixBuffer, 0);
	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;
	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	////layout buffer set up
	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(layoutBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	// Get a pointer to the data in the constant buffer.
	layoutPtr = (layoutBufferType*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	int x = width / 3;
	layoutPtr->pts = XMFLOAT3(x-20, x + x+20, height/2);
	layoutPtr->radius = 280.0f;
	// Unlock the constant buffer.
	deviceContext->Unmap(layoutBuffer, 0);
	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;
	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &layoutBuffer);


	deviceContext->PSSetShaderResources(0, 1, &texture);
}
void PostProcessingShader::render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	// Base render function.
	BaseShader::render(deviceContext, indexCount);
}


