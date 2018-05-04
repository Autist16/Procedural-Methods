// texture shader.cpp
#include "TerrainShader.h"


TerrainShader::TerrainShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"terrain_vs.cso", L"terrain_ps.cso");
	createRasterState(device, false);
}


TerrainShader::~TerrainShader()
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

	//release additonal buffers
	if (pixelLightBuffer)
	{
		pixelLightBuffer->Release();
		pixelLightBuffer = 0;
	}
	if (timeBuffer)
	{
		timeBuffer->Release();
		timeBuffer = 0;
	}
	//Release base shader components
	BaseShader::~BaseShader();
}


void TerrainShader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	//buffer descs
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC pixelLightBufferDesc;
	D3D11_BUFFER_DESC timeBufferDesc;

	//sampler descs
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_SAMPLER_DESC samplerClampedDesc;

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

	//create pixel light buffer desc
	pixelLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelLightBufferDesc.ByteWidth = sizeof(LightBufferType);
	pixelLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelLightBufferDesc.MiscFlags = 0;
	pixelLightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&pixelLightBufferDesc, NULL, &pixelLightBuffer);

	//create time buffer desc
	timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timeBufferDesc.ByteWidth = sizeof(TimeBufferType);
	timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timeBufferDesc.MiscFlags = 0;
	timeBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&timeBufferDesc, NULL, &timeBuffer);

	//samplers
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
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
	renderer->CreateSamplerState(&samplerClampedDesc, &sampleStateClamp);

	samplerClampedDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerClampedDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerClampedDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerClampedDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerClampedDesc.MipLODBias = 0.0f;
	samplerClampedDesc.MaxAnisotropy = 1;
	samplerClampedDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerClampedDesc.BorderColor[0] = 0;
	samplerClampedDesc.BorderColor[1] = 0;
	samplerClampedDesc.BorderColor[2] = 0;
	samplerClampedDesc.BorderColor[3] = 0;
	samplerClampedDesc.MinLOD = 0;
	samplerClampedDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerClampedDesc, &sampleStateClamp);
}


void TerrainShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, Light *mainLight, ID3D11ShaderResourceView* texture[], float timeP)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightBufferType* pixelLightPtr;
	TimeBufferType* timePtr;

	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj, tLightViewMatrix, tLightProjMatrix;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	tLightViewMatrix = XMMatrixTranspose(mainLight->getViewMatrix());
	tLightProjMatrix = XMMatrixTranspose(mainLight->getProjectionMatrix());

	///// ///// ///// ///// ///// ///// ///// ///// /////
	///// ///// //// VERTEX SHADER DATA //// ///// /////
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	//pass to data 
	dataPtr->world = tworld;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView = tLightViewMatrix;
	dataPtr->lightProjection = tLightProjMatrix;
	deviceContext->Unmap(matrixBuffer, 0); // Unlock the constant buffer.
	// Set constant buffer in the vertex shader.
	bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	//set up time buffer in vertex shader
	result = deviceContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timePtr = (TimeBufferType*)mappedResource.pData;
	//pass data to buffer
	timePtr->time = timeP;
	timePtr->timePadding = XMFLOAT3(0.0f, 0.0f, 0.0f);
	deviceContext->Unmap(timeBuffer, 0);
	// Set constant buffer in the vertex shader.
	bufferNumber = 1;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &timeBuffer);

	///// ///// ///// ///// ///// ///// ///// //// /////
	///// ///// //// PIXEL SHADER DATA //// ///// /////
	// set up light buffer in pixel shader
	result = deviceContext->Map(pixelLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	pixelLightPtr = (LightBufferType*)mappedResource.pData;
	//pass data to buffer
	pixelLightPtr->ambient = mainLight->getAmbientColour();
	pixelLightPtr->diffuse = mainLight->getDiffuseColour();
	pixelLightPtr->direction = mainLight->getDirection();
	pixelLightPtr->padding = 0.0f;
	//unlock and set position in pixel shader
	deviceContext->Unmap(pixelLightBuffer, 0);
	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &pixelLightBuffer);


	//pass textures to pixel shader
	deviceContext->PSSetShaderResources(0, 5, texture);

}
void TerrainShader::render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateClamp);
	deviceContext->RSSetState(rasterState);
	// Base render function.
	BaseShader::render(deviceContext, indexCount);
}

void TerrainShader::createRasterState(ID3D11Device* device, bool wire)
{
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	if (!wire)
	{
		rasterDesc.FillMode = D3D11_FILL_SOLID;
	}
	else
	{
		rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	}
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	rasterDesc.AntialiasedLineEnable = true;

	// Create the rasterizer state from the description we just filled out.
	device->CreateRasterizerState(&rasterDesc, &rasterState);

}

void TerrainShader::loadVertexShader(WCHAR* filename)
{
	ID3DBlob* vertexShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[4];
	unsigned int numElements;

	vertexShaderBuffer = 0;
	// check file extension for correct loading function.
	std::wstring fn(filename);
	std::string::size_type idx;
	std::wstring extension;

	idx = fn.rfind('.');

	if (idx != std::string::npos)
	{
		extension = fn.substr(idx + 1);
	}
	else
	{
		// No extension found
		MessageBox(hwnd, L"Error finding vertex shader file", L"ERROR", MB_OK);
		exit(0);
	}

	// Load the texture in.
	if (extension != L"cso")
	{
		MessageBox(hwnd, L"Incorrect vertex shader file type", L"ERROR", MB_OK);
		exit(0);
	}

	// Reads compiled shader into buffer (bytecode).
	HRESULT result = D3DReadFileToBlob(filename, &vertexShaderBuffer);
	if (result != S_OK)
	{
		MessageBox(NULL, filename, L"File ERROR", MB_OK);
		exit(0);
	}

	// Create the vertex shader from the buffer.
	renderer->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the MeshClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TEXCOORD";
	polygonLayout[3].SemanticIndex = 1;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	renderer->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layout);

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
}



