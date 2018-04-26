#ifndef _OWNSPHEREMESH_H_
#define _OWNSPHEREMESH_H_

#include "../DXFramework/BaseMesh.h"

using namespace DirectX;

class OwnSphere :public BaseMesh
{
public:
	OwnSphere(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~OwnSphere();

protected:
	void initBuffers(ID3D11Device* device);
	void setUpTexureCoords();
	void increaseTextureCoord(int x);

	XMFLOAT2* textCoords;

};
#endif // !_OWNSPHEREMESH_H_

