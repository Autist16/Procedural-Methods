#include "NoiseGeneration.h"

NoiseGeneration::NoiseGeneration(int res)
{
	//create resources
	seedValues2D = new float[res*res];
	
	//populate seed vals
	for (int i = 0; i < res*res; i++)
	{
		float v = (rand() % 10) -5;
		seedValues2D[i] = (v / 10);
	}
}