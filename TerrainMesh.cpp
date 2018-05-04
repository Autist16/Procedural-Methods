#include "TerrainMesh.h"
#include <ctime>

// Initialise buffer and load texture.
TerrainMesh::TerrainMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int res)
{
	resolution = res;
	noAreas = 5;
	VertexDataBuffer = 0;
	
	seedVals = new float[res];
	noiseSeeds = new float[res*res];
	
	for (int i = 0; i < res; i++)
	{
		float n = (rand() %2);
		seedVals[i] = n;
	}
	for (int j = 0; j < res*res; j++)
	{
		float n = (rand() % 3);
		noiseSeeds[j] = n;
	}
	
	CreateHeightMap(device);
	noiseGen2D();
	genericHeight();
	CreateAreaMap();
	mountainCalculation();
	initBuffers(device);
}

// Release resources.
TerrainMesh::~TerrainMesh()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();

	if (VertexDataBuffer)
	{
		VertexDataBuffer->Release();
		VertexDataBuffer = 0;
	}
}

// Generate plane (including texture coordinates and normals).
void TerrainMesh::initBuffers(ID3D11Device* device)
{
	//calcaulate normals
	calculateNormals();

	VertexType* vertices;
	unsigned long* indices;
	int index, i, j;
	float positionX, positionZ, u, v, increment;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Calculate the number of vertices in the terrain mesh.
	vertexCount = (resolution - 1) * (resolution - 1) * 8;


	indexCount = vertexCount;
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];


	index = 0;
	// UV coords.
	u = 0;
	v = 0;
	increment =1.f;
	int counter = 0; //used to generate the fishbone pattern
	int index1, index2, index3, index4;

	for (j = 0; j<(resolution - 1); j++)
	{
		for (i = 0; i<(resolution - 1); i++)
		{
			counter += 1;
			index1 = (resolution * j) + i;          // Bottom left.
			index2 = (resolution * j) + (i + 1);      // Bottom right.
			index3 = (resolution * (j + 1)) + i;      // Upper left.
			index4 = (resolution * (j + 1)) + (i + 1); //upder right;

			if (counter % 2 == 0)
			{
				// Upper left.
				positionX = (float)i;
				positionZ = (float)(j);

				vertices[index].position = heightMap[index3].postion;
				vertices[index].texture = XMFLOAT2(u, v);
				vertices[index].normal = heightMap[index3].normals;
				vertices[index].area = XMFLOAT2(heightMap[index3].areaType);
				indices[index] = index;
				index++;
				
				// lower left
				positionX = (float)(i);
				positionZ = (float)(j + 1);

				vertices[index].position = heightMap[index1].postion;
				vertices[index].texture = XMFLOAT2(u, v + increment);
				vertices[index].normal = heightMap[index1].normals;
				vertices[index].area = XMFLOAT2(heightMap[index1].areaType);
				indices[index] = index;
				index++;

				// Upper right.
				positionX = (float)(i + 1);
				positionZ = (float)(j + 1);

				vertices[index].position = heightMap[index4].postion;
				vertices[index].texture = XMFLOAT2(u + increment, v);
				vertices[index].normal = heightMap[index4].normals;
				vertices[index].area = XMFLOAT2(heightMap[index4].areaType);
				indices[index] = index;
				index++;

				// Upper right.
				positionX = (float)(i + 1);
				positionZ = (float)(j + 1);

				vertices[index].position = heightMap[index4].postion;
				vertices[index].texture = XMFLOAT2(u + increment, v);
				vertices[index].normal = heightMap[index4].normals;
				vertices[index].area = XMFLOAT2(heightMap[index4].areaType);
				indices[index] = index;
				index++;

				// lower left
				positionX = (float)(i);
				positionZ = (float)(j + 1);

				vertices[index].position = heightMap[index1].postion;
				vertices[index].texture = XMFLOAT2(u, v + increment);
				vertices[index].normal = heightMap[index1].normals;
				vertices[index].area = XMFLOAT2(heightMap[index1].areaType);
				indices[index] = index;
				index++;

				// Bottom right

				positionX = (float)(i + 1);
				positionZ = (float)(j);

				vertices[index].position = heightMap[index2].postion;
				vertices[index].texture = XMFLOAT2(u + increment, v + increment);
				vertices[index].normal = heightMap[index2].normals;
				vertices[index].area = XMFLOAT2(heightMap[index2].areaType);
				indices[index] = index;
				index++;

			}
			else
			{
				// Upper right.
				positionX = (float)(i + 1);
				positionZ = (float)(j + 1);

				//vertices[index].position = XMFLOAT3(positionX, heightMap[index4].postion.y, positionZ);
				vertices[index].position = heightMap[index4].postion;
				vertices[index].texture = XMFLOAT2((u + increment), (v));
				vertices[index].normal = heightMap[index4].normals;
				vertices[index].area = XMFLOAT2(heightMap[index4].areaType);
				indices[index] = index;
				index++;

				// Upper left.
				positionX = (float)i;
				positionZ = (float)(j);

				//vertices[index].position = XMFLOAT3(positionX, heightMap[index3].postion.y, positionZ);
				vertices[index].position = heightMap[index3].postion;
				vertices[index].texture = XMFLOAT2(u, v);
				vertices[index].normal = heightMap[index3].normals;
				vertices[index].area = XMFLOAT2(heightMap[index3].areaType);
				indices[index] = index;
				index++;

				// Bottom right

				positionX = (float)(i + 1);
				positionZ = (float)(j);

				//vertices[index].position = XMFLOAT3(positionX, heightMap[index2].postion.y, positionZ);
				vertices[index].position = heightMap[index2].postion;
				vertices[index].texture = XMFLOAT2((u+increment), v + increment);
				vertices[index].normal = heightMap[index2].normals;
				vertices[index].area = XMFLOAT2(heightMap[index2].areaType);
				indices[index] = index;
				index++;

				// Bottom right

				positionX = (float)(i + 1);
				positionZ = (float)(j);

				//vertices[index].position = XMFLOAT3(positionX, heightMap[index2].postion.y, positionZ);
				vertices[index].position = heightMap[index2].postion;
				vertices[index].texture = XMFLOAT2(u + increment, v + increment);
				vertices[index].normal = heightMap[index2].normals;
				vertices[index].area = XMFLOAT2(heightMap[index2].areaType);
				indices[index] = index;
				index++;

				// Upper left.
				positionX = (float)i;
				positionZ = (float)(j);

				//vertices[index].position = XMFLOAT3(positionX, heightMap[index3].postion.y, positionZ);
				vertices[index].position = heightMap[index3].postion;
				vertices[index].texture = XMFLOAT2(u, v);
				vertices[index].normal = heightMap[index3].normals;
				vertices[index].area = XMFLOAT2(heightMap[index3].areaType);
				indices[index] = index;
				index++;

				// lower left
				positionX = (float)(i);
				positionZ = (float)(j + 1);


				//vertices[index].position = XMFLOAT3(positionX, heightMap[index1].postion.y, positionZ);
				vertices[index].position = heightMap[index1].postion;
				vertices[index].texture = XMFLOAT2(u, v + increment);
				vertices[index].normal = heightMap[index1].normals;
				vertices[index].area = XMFLOAT2(heightMap[index1].areaType);
				indices[index] = index;
				index++;

			}
			
		}
	}

	// Set up the description of the static vertex buffer.
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
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
	
}

void TerrainMesh::sendData(ID3D11DeviceContext* deviceContext)
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

void TerrainMesh::calculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	XMFLOAT3 vertex1, vertex2, vertex3, vector1, vector2, vector3, sum;
	float length;
	XMFLOAT3* normals;

	//create temp array to hold un normalised data
	normals = new XMFLOAT3[(resolution - 1) * (resolution - 1)];

	for (j = 0; j < resolution - 1; j++)
	{
		for (i = 0; i < resolution - 1; i++)
		{
			index1 = (j * resolution) + i;
			index2 = (j * resolution) + (i + 1);
			index3 = ((j + 1) * resolution) + i;

			// Get three vertices from the face.
			vertex1 = heightMap[index1].postion;

			vertex2 = heightMap[index2].postion;

			vertex3 = heightMap[index3].postion;

			// Calculate the two vectors for this face.
			vector1 = XMFLOAT3((vertex1.x- vertex3.x),(vertex1.y - vertex3.y), (vertex1.z - vertex3.z));
			vector2 = XMFLOAT3((vertex3.x - vertex2.x), (vertex3.y - vertex2.y), (vertex3.z - vertex2.z));
			

			index = (j * (resolution - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1.y * vector2.z) - (vector1.z * vector2.y);
			normals[index].y = (vector1.z * vector2.x) - (vector1.x * vector2.z);
			normals[index].z = (vector1.x * vector2.y) - (vector1.y * vector2.x);
		}
	}
	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for (j = 0; j<resolution; j++)
	{
		for (i = 0; i<resolution; i++)
		{
			// Initialize the sum.
			sum = XMFLOAT3(0.0f, 0.0f, 0.0f);

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (resolution - 1)) + (i - 1);

				sum.x += normals[index].x;
				sum.y += normals[index].y;
				sum.z += normals[index].z;
				count++;
			}

			// Bottom right face.
			if ((i < (resolution - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (resolution - 1)) + i;

				sum.x += normals[index].x;
				sum.y += normals[index].y;
				sum.z += normals[index].z;
				count++;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (resolution- 1)))
			{
				index = (j * (resolution - 1)) + (i - 1);

				sum.x += normals[index].x;
				sum.y += normals[index].y;
				sum.z += normals[index].z;
				count++;
			}

			// Upper right face.
			if ((i < (resolution - 1)) && (j < (resolution - 1)))
			{
				index = (j * (resolution - 1)) + i;

				sum.x += normals[index].x;
				sum.y += normals[index].y;
				sum.z += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum.x = (sum.x / (float)count);
			sum.y = (sum.y / (float)count);
			sum.z = (sum.z / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum.x * sum.x) + (sum.y * sum.y) + (sum.z * sum.z));

			// Get an index to the vertex location in the height map array.
			index = (j * resolution) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			XMFLOAT3 norm = XMFLOAT3((sum.x / length), (sum.y / length), (sum.z / length));
			
			heightMap[index].normals = norm;
		}
	}
}

void TerrainMesh::RestartTerrain(ID3D11Device* device)
{
	float h = 0.0f;
	int index = 0;
	int res = resolution;
	//initalise data in the height map
	for (int j = 0; j < resolution; j++)
	{
		for (int i = 0; i < resolution; i++)
		{
			heightMap[index].postion = XMFLOAT3(i, h, j);
			heightMap[index].normals = XMFLOAT3(0.0f, 1.0, 0.0f);
			heightMap[index].areaType = XMFLOAT2(2.0f, 0.0f);
			index++;
		}
	}

	//repopulate seedVals for noise
	for (int i = 0; i < res; i++) //1d noise
	{
		float v = rand() % 9 + 1;
		seedVals[i] = v / 10;
	}
	for (int j = 0; j < res*res; j++) //2d noise
	{
		int n = (rand() % 3);
		noiseSeeds[j] = n;
	}
	//clear area map
	int area = 0;
	int radius;
	XMFLOAT2 centrePoint;

	for (int i = 0; i < noAreas; i++)
	{
		radius = (rand() % 50 + 20);
		centrePoint.x = (rand() % resolution + 4);
		centrePoint.y = (rand() % resolution + 6);

		areaMap[i].radius = radius;
		areaMap[i].centre = centrePoint;

		area = rand() % 2;
		areaMap[i].areaType = area;
	}
	noiseGen2D();
	genericHeight();
	mountainCalculation();
	initBuffers(device);
}

//// //// //// //// //// //// //// //// //// //// //// //// //// //// //// ////
//// //// //// ////  CREATE  REASOURCES //// //// //// //////// //// //// ////
void TerrainMesh::CreateHeightMap(ID3D11Device* device)
{
	int index =0;
	float height = 0.0f;

	heightMap = new HeightMapType[(resolution * resolution)];
	//initalise data in the height map
	for (int j = 0; j < resolution; j++)
	{
		for (int i = 0; i < resolution; i++)
		{
			heightMap[index].postion = XMFLOAT3(i, height, j);
			heightMap[index].normals = XMFLOAT3(0.0f, 1.0, 0.0f);
			heightMap[index].areaType = XMFLOAT2(2.0f,0.0f);
			index++;
		}
	}
}

void TerrainMesh::CreateAreaMap()
{
	srand(std::time(0));
	areaMap = new AreaMapType[noAreas];
	int area = 0;
	int radius;
	XMFLOAT2 centrePoint;

	for (int i = 0; i < noAreas; i++)
	{
		radius = (rand() % 50 + 20);
		centrePoint.x = (rand() % resolution + 4);
		centrePoint.y = (rand() % resolution + 6);

		areaMap[i].radius = radius;
		areaMap[i].centre = centrePoint;

		area = rand() % 2;
		areaMap[i].areaType = area;
	}

}

//generate noise for 
void TerrainMesh::noiseGen2D()
{
	int nOutputSize = resolution*resolution;
	noise2D = new float[nOutputSize];
	int width = resolution;
	int height = resolution;

	//create noise for general terrian shape
	int noOctaves = 8; // for now
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float noiseVals = 0.0f;
			float scalingVal = 1.0f;
			float scaleAcu = 0.0f; //used to check values are valid

			for (int oc = 0; oc < noOctaves; oc++) //for each ocatave
			{
				int nPitch = height >> oc;
				int nSampleX1 = (x / nPitch)*nPitch;
				int nSampleY1 = (y / nPitch)*nPitch;

				int nSampleX2 = (nSampleX1 + nPitch) % width; //wrap round and tesselate
				int nSampleY2 = (nSampleY1 + nPitch) % height; //wrap round and tesselate

				float fBlendX = (float(x - nSampleX1) / (float)nPitch); //how far into pitch
				float fBlendY = (float(y - nSampleY1) / (float)nPitch); //how far into pitch

																		//interpolate between values
				float fSampleT;
				fSampleT = (1.0f - fBlendX)*noiseSeeds[nSampleY1*width + nSampleX1] + fBlendX*noiseSeeds[nSampleY1*width + nSampleX2];
				
				float fSampleB = (1.0f - fBlendX)*noiseSeeds[nSampleY2*width + nSampleX1] + fBlendX*noiseSeeds[nSampleY2*width + nSampleX2];

				
				//update values
				noiseVals += (fBlendY *(fSampleB - fSampleT) + fSampleT)* scalingVal;
				scaleAcu += scalingVal;
				scalingVal /= 2;
			}
			noise2D[y * width + x] = noiseVals / scaleAcu;
		}
	}
}

void TerrainMesh::mountainCalculation()
{
	int nOutputSize = resolution;
	mountianNoise = new XMFLOAT2[nOutputSize];
	mountainOutput = new XMFLOAT2[nOutputSize];

	for (int i = 0; i < nOutputSize; i++)
	{
		float n = rand() % 2;
		float m = rand() % 3;
		mountianNoise[i] = XMFLOAT2(n, m);
	}
	 
	int noOctaves = 8;

	//generate patterns
	for (int x = 0; x < nOutputSize; x++)
	{
		XMFLOAT2 noiseVal = XMFLOAT2(0.f, 0.f);
		float scalingVal = 1.0f;
		float acc = 0.0f;
		//generate noise over each octave
		for (int oc = 0; oc < noOctaves; oc++)
		{
			int pitch = nOutputSize >> oc;
			int nSample1 = (x / pitch)*pitch;
			int nSample2 = (nSample1 + pitch) % nOutputSize;

			float blend = (float(x - nSample1)) / (float)pitch;
			XMFLOAT2 sample;
			sample.x = (1.0f - blend)*mountianNoise[nSample1].x + blend*mountianNoise[nSample2].x;
			sample.y = (1.0f - blend)*mountianNoise[nSample1].y + blend*mountianNoise[nSample2].y;

			noiseVal.x += sample.x*scalingVal;
			noiseVal.y += sample.y*scalingVal;
			//update values
			acc += scalingVal;
			scalingVal = scalingVal / 2.0f;

		}
		//scale
		mountainOutput[x].x = noiseVal.x / acc;
		mountainOutput[x].y = noiseVal.y / acc;
	}

}

//area calculations

void TerrainMesh::calculateEdgeDistance(int i, int a, int x, int z)
{
	float measureX, measureZ, dist, centreDist;

	measureX = areaMap[a].centre.x - x;
	measureZ = areaMap[a].centre.y - z;
	dist = sqrtf((measureX*measureX) + (measureZ*measureZ));
	 
	float cx, cz;
	cx = areaMap[a].centre.x + areaMap[a].radius;
	cz = areaMap[a].centre.y + areaMap[a].radius;
	centreDist = sqrtf((cx*cx) + (cz*cz));;
	//normalise this value
	dist = dist / centreDist;

	//check next area type
	float xo, yo;
	xo = areaMap[a].centre.x + areaMap[a].radius + 1; //values used to check area
	yo = areaMap[a].centre.y + areaMap[a].radius + 1;

	//check values within range
	if (xo < 0)
	{
		xo = 0;
	}
	if (yo < 0)
	{
		yo = 0;
	}
	if (xo > resolution - 1)
	{
		xo = resolution - 1;
	}
	if (yo > resolution - 1)
	{
		yo = resolution - 1;
	}

	//find area type of surrounding space
	int index = (resolution * yo) + xo;
	
	float area2 = heightMap[index].areaType.x;
	heightMap[i].areaType.y = dist +area2;
}

void TerrainMesh::ApplyAreaMap(ID3D11Device* device)
{
	int index = 0;
	int pointType = 10;
	float edgeDist;

	for (int j = 0; j < resolution; j++)
	{
		for (int i = 0; i < resolution; i++)
		{
			for (int a = 0; a < noAreas; a++)
			{
				float xa = (j - areaMap[a].centre.x);
				float yb = (i - areaMap[a].centre.y);
				float eq = (xa*xa) + (yb*yb); 

				if (eq <= (areaMap[a].radius*areaMap[a].radius))
				{
					if (heightMap[index].areaType.x != 3)
					{
						heightMap[index].areaType.x = areaMap[a].areaType;
					}

					//get area type for this zone
					pointType =(int) heightMap[index].areaType.x;

					//calculate edge distance for this point (used for texture blending)
					calculateEdgeDistance(index, a, j, i);

					if (pointType == 1)
					{
						RandomHeightGen(i, j, a);
					}
				}
			}

			index++;
		}
	}

	index = 0;

	waterArea();
	applyMountians();
	initBuffers(device);
}

//// //// //// //// //// //// //// //// //// //// //// //// //// //// //// ////
//// //// //// //// HEIGHT MAP ALTERING FUNCTIONS //// //// //// //// //// ////
void TerrainMesh::genericHeight()
{
	int index;
	float height;
	for (int j = 0; j < resolution; j++)
	{
		for (int i = 0; i < resolution; i++)
		{
			index = (resolution * j) + i;
			
			heightMap[index].postion.y = noise2D[index] * 20;
		}
	}
}
void TerrainMesh::RandomHeightGen(int i, int j, int a)
{
	int index;
	int height = 0.0f;
	index = (resolution * j) + i;
	float h = ((rand() % 7 +1)+3)/10;
	XMFLOAT2 c = areaMap[a].centre;
	float blendVal = heightMap[index].areaType.y - (int)heightMap[index].areaType.y;
	if (blendVal < 0.05f)
	{
		blendVal = 0.075f;
	}
	h /= blendVal;
	heightMap[index].postion.y += h+1;

}

void TerrainMesh::waterArea()
{
	int index = -1;
	float waterHeight = 200.f;
	//check surrounding areas for height;
	for (int j = 0; j < resolution; j++)
	{
		for (int i = 0; i < resolution; i++)
		{
			index++;
			if (heightMap[index].areaType.x == 0.0f)
				{
				int checkVal = 0.0f;
					//check this point for  water height
					if (heightMap[index].postion.y < waterHeight)
					{
						waterHeight = heightMap[index].postion.y;
					}

					if (j >2)
					{
						if (heightMap[resolution*(j - 2) + i].postion.y < waterHeight)
						{
							waterHeight = heightMap[resolution*(j - 2) + i].postion.y;
						}
					}
					if (i > 2)
					{
						if (heightMap[(resolution*j) + i - 2].postion.y < waterHeight)
						{
							waterHeight = heightMap[(resolution*j) + i - 2].postion.y;
						}
					}

					if (j < resolution - 2)
					{
						if (heightMap[resolution*(j + 2) + i].postion.y < waterHeight)
						{
							waterHeight = heightMap[resolution*(j + 2) + i].postion.y;
						}
					}
					if (i < resolution - 2)
					{
						if (heightMap[(resolution*j) + i +2].postion.y < waterHeight)
						{
							waterHeight = heightMap[(resolution*j) + i + 2].postion.y;
						}
					}
			}
		}
	}
	
	index = 0;
	//finally apply water height to points
	for (int j = 0; j < resolution; j++)
	{
		for (int i = 0; i < resolution; i++)
		{
			if (heightMap[index].areaType.x == 0.0f)
			{
				heightMap[index].postion.y = waterHeight;
			}
			index++;
		}
	}
}

void TerrainMesh::smooth(ID3D11Device* device)
{
	int index = 0;
	for (int j = 0; j < (resolution); j++)
	{
		for (int i = 0; i < (resolution); i++)
		{
			index = (resolution * j) + i;
			int samples = 1;
			float temp = heightMap[index].postion.y;
			if (j != 0)
			{
				temp += heightMap[resolution*(j - 1) + i].postion.y;
				samples += 1;
			}
			if (i != 0)
			{
				temp += heightMap[(resolution*j) + i - 1].postion.y;
				samples += 1;
			}

			if (j != resolution - 1)
			{
				temp += heightMap[resolution*(j + 1) + i].postion.y;
				samples += 1;
			}
			if (i != resolution - 1)
			{
				temp += heightMap[(resolution*j) + i + 1].postion.y;
				samples += 1;
			}

				float average = temp / samples;

				heightMap[index].postion.y = average;
		}
	}

	initBuffers(device);

}

void TerrainMesh::AddFaultLine(ID3D11Device* device)
{
	int nOutputSize = resolution;
	faultOutput1D = new float[resolution];
	int noOctaves = 8;

	//create noise for fault line
	for (int x = 0; x < nOutputSize; x++)
	{
		float noiseVal = 0.0f;
		float scalingVal = 1.0f;
		float acc = 0.0f;
		//generate noise over each octave
		for (int oc = 0; oc < noOctaves; oc++)
		{
			int pitch = nOutputSize >> oc;
			int nSample1 = (x / pitch)*pitch;
			int nSample2 = (nSample1 + pitch) % nOutputSize;
			
			float blend = (float(x - nSample1)) / (float)pitch;
			float sample = (1.0f - blend)*seedVals[nSample1] + blend*seedVals[nSample2];
			
			noiseVal += sample*scalingVal;
			//update values
			acc += scalingVal;
			scalingVal = scalingVal / 2.0f;
			
			
		}
		//scale
		faultOutput1D[x] = noiseVal/acc;
		
	}
	int index;
	//apply fault in 1 i dr
	for (int j = 0; j < resolution; j++)
	{
		float k = faultOutput1D[j] *(resolution-1);
		index = (resolution * (int)k) + j;
		heightMap[index].postion.y -= (rand() % 3) + 1;
		for (int i = (int)k; i >= 0; i--)
		{
			index = (resolution*i) + j;
			heightMap[index].postion.y -= 12.0;
			if (i >= k - 11 && i <= k)
			{
				heightMap[index].areaType.x = 3.0;
			}
		}
		for (int i = (int)k -10; i >= 0; i--)
		{
			index = (resolution*i) + j;
			heightMap[index].postion.y += 5.0;
		}
		
	}
}
void TerrainMesh::applyMountians()
{
	int index;
	//apply mountains in 1 d
	for (int j = 0; j < resolution; j++)
	{
		float k = mountainOutput[j].x * (resolution - 1);
		index = (resolution * (int)k) + j;
		float h = mountainOutput[j].y * 30;
		heightMap[index].postion.y += h;
		if(h > 25.f)
		{
			heightMap[index].areaType = XMFLOAT2(4.0f, 3.0f);
		}
		else if (h > 20.0f && h < 26.f)
		{
			heightMap[index].areaType = XMFLOAT2(3.0f, 4.0f);
		}
		else {
			heightMap[index].areaType = XMFLOAT2(3.0f, 3.0f);
		}
		for (int i = k - 20; i < k + 20; i++)
		{
			if (i > 0 && i < resolution - 1)
			{
				if (i != k)
				{
					index = (resolution*i) + j;
					heightMap[index].postion.y += h / 8;
					heightMap[index].areaType = XMFLOAT2(3.0f, 3.0f);
				}
			}
		}
		for (int i = k - 10; i < k + 10; i++)
		{
			if (i > 0 && i < resolution - 1 && i!=k)
			{
					index = index = (resolution*i) + j;
					heightMap[index].postion.y += h / 6;
			}
		}
		for (int i = k - 5; i < k + 5; i++)
		{
			if (i > 0 && i < resolution - 1)
			{
				if (i != k)
				{
					index = index = (resolution*i) + j;
					heightMap[index].postion.y += h / 4;
					float vary = (rand() %8) - 4;
					if (heightMap[index].postion.y > (20.f+vary))
					{
						heightMap[index].areaType = XMFLOAT2(4.0f,3.0f);
					}
				}
			}
		}
	}
}
		

