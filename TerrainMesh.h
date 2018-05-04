//terrain mesh class
#ifndef _TERRAINMESH_H_
#define _TERRAINMESH_H_

#include "../DXFramework/BaseMesh.h"
#include <vector>
using std::vector;

class TerrainMesh : public BaseMesh
{
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT2 area;
	};

	struct HeightMapType
	{
		XMFLOAT3 postion;
		XMFLOAT3 normals;
		XMFLOAT2 areaType;
	};

	struct AreaMapType
	{
		XMFLOAT2 centre;
		float radius;
		int areaType;
	};

public:
	TerrainMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int res);
	~TerrainMesh();
	void sendData(ID3D11DeviceContext* deviceContext);
	void RestartTerrain(ID3D11Device* device);
	//area calc
	void ApplyAreaMap(ID3D11Device* device);
	
	//heighht map altering
	void smooth(ID3D11Device* device);
	void AddFaultLine(ID3D11Device* device);
	
	
protected:
	void initBuffers(ID3D11Device* device);
	void calculateNormals();
	//create reasources
	void CreateHeightMap(ID3D11Device* device);
	void CreateAreaMap();
	//generate noise
	void noiseGen2D();
	void mountainCalculation();
	//area calculations
	void calculateEdgeDistance(int i, int a, int x, int z);
	
	//heighht map altering
	void genericHeight();
	void RandomHeightGen(int i, int j, int a);
	void waterArea();
	void applyMountians();
	

	
	//structs variables and buffers
	HeightMapType* heightMap;
	AreaMapType* areaMap;
	ID3D11Buffer* VertexDataBuffer;

private:
	float* faultOutput1D;
	float* noise2D;
	float* seedVals;
	float* noiseSeeds;

	XMFLOAT2* mountainOutput;
	XMFLOAT2* mountianNoise;

	int resolution;
	int noAreas;

};

#endif

//noise creation algorithm taken from
// https://www.youtube.com/watch?v=6-0UaeJBumA