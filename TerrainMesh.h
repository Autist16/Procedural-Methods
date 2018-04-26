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
		float lowPoint;
	};

public:
	TerrainMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int res);
	~TerrainMesh();
	void sendData(ID3D11DeviceContext* deviceContext);
	
	void ApplyAreaMap(ID3D11Device* device);
	void RestartTerrain(ID3D11Device* device);
	
	void smooth(ID3D11Device* device);
	void AddFaultLine(ID3D11Device* device);
	void waterArea();
	
	//getters
	float getHeightPoint(XMFLOAT3 point);
	int getAreatype(int i)
	{
		return areaMap[i].areaType;
	}
	XMFLOAT2 getAreaCentre(int i)
	{
		return areaMap[i].centre;
	}
	float getAreaRadius(int i)
	{
		return areaMap[i].radius;
	}
protected:
	void initBuffers(ID3D11Device* device);

	void CreateHeightMap(ID3D11Device* device);
	void calculateNormals();
	void calculateEdgeDistance(int i, int a, int x, int z);
	void mountainCalculation();
	void CreateAreaMap();
	void genericHeight();
	void RandomHeightGen(int i, int j, int a);
	void applyMountians();

	int resolution;
	int noAreas;

	void noiseGen2D();

	HeightMapType* heightMap;
	AreaMapType* areaMap;
	ID3D11Buffer* VertexDataBuffer;

private:
	float* faultOutput1D;
	float* noise2D;
	float* mountainPattern;
	float* seedVals;
	float* noiseSeeds;

	XMFLOAT2* mountainOutput;
	XMFLOAT2* mountianNoise;

};

#endif
