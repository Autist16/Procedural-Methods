#include"cameraControl.h"

CameraControl::CameraControl(int res)
{
	terrainSize = res;
	verticalDist = 2;
	camBounds = XMFLOAT2(0, res);
}

void CameraControl::RaycastDown(XMFLOAT3 currentPos, float tHeight)
{
	currentPos.y = tHeight + verticalDist;

	cameraPos = currentPos;
}

void CameraControl::checkInBounds(XMFLOAT3 currentPos)
{
	//camBounds X is min value
	//camBounds y is max value
	//for both x and z axis of camera positioning

	//check min values
	if (currentPos.x < camBounds.x)
	{
		currentPos.x = camBounds.x;
	}
	if (currentPos.z < camBounds.x)
	{
		currentPos.z = camBounds.x;
	}
	//check max values
	if (currentPos.x > camBounds.y)
	{
		currentPos.x = camBounds.y;
	}
	if (currentPos.z > camBounds.y);
	{
		currentPos.z = camBounds.y;
	}

	cameraPos = currentPos;
}