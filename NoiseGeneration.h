#ifndef _NOISE_GENERATION_H_
#define _NOISE_GENERATION_H_

#include "../DXFramework/BaseApplication.h"
#include <vector>
using std::vector;


class NoiseGeneration
{
public:
	NoiseGeneration(int res);
	~NoiseGeneration();
	void createNoise();
	float getNoise();
private:
	int resolution;
	float* seedValues2D;

	float* noise2D;
	
};
#endif

