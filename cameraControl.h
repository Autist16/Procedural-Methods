//camera control functions
//used to limit how the camera moves across the terrain without altering the frameworks camera stuff
#ifndef _CAMERACONTROL_H_
#define _CAMERACONTROL_H_

#include"../DXFramework/Camera.h"

class CameraControl
{
public:
	CameraControl(int res);
	
	void RaycastDown(XMFLOAT3 currentPos, float terrainHeight);
	void checkInBounds(XMFLOAT3 currentPos);

	//getters & setters
	XMFLOAT3 getCameraPos()
	{
		return cameraPos;
	}
protected:
	int terrainSize;
	int verticalDist;
	XMFLOAT3 cameraPos;
	XMFLOAT2 camBounds;
};

#endif
