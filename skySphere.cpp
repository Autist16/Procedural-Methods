#include "skySphere.h"

SkysphereMesh::SkysphereMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int res)
{
	resolution = res;
	initBuffers(device);
}
SkysphereMesh::~SkysphereMesh()
{
	BaseMesh::~BaseMesh();
}

void SkysphereMesh::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	//6 vertices per quad ....
	vertexCount = 3;
	indexCount = vertexCount;

	//create vertex and index array
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	int v = 0; //vertex counter
	int i = 0;	//index counter
	
	//sphere details
	int radius = 1;
	int steps = 10;
	float angle = 360 / steps;
	float cAngle = 0.0f;
	float nAngle = angle;
	//face 1
		//bottom left
	vertices[v].position = XMFLOAT3(0, 0,0);
	vertices[v].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[v].normal = XMFLOAT3(0, 0, 1);
	vertices[v].texture2 = XMFLOAT2(0.5, 0.0f);
	v++;
	i++;
		//top right
	vertices[v].position = XMFLOAT3(1, 1, 0);
	vertices[v].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[v].normal = XMFLOAT3(0, 0, 1);
	vertices[v].texture2 = XMFLOAT2(1.5f, 1.0f);
	v++;
	i++;
		//top left
	vertices[v].position = XMFLOAT3(0, 1, 0);
	vertices[v].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[v].normal = XMFLOAT3(0, 0, 1);
	vertices[v].texture2 = XMFLOAT2(0.5f, 1.0f);
	v++;
	i++;


	//create buffers
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = 0;
	}
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	if (indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = 0;
	}
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}

void SkysphereMesh::sendData(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);

	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
