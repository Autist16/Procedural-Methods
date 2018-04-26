// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	//BaseApplication::BaseApplication();
	terrainShader = nullptr;
	colourShader = nullptr;
	terrainMesh = nullptr;
	skyMesh = nullptr;

	cameraController = nullptr;
	patternGenerator = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);

	//set up inital camera positon
	camera->setPosition(6, 10, 6);

	// Create Meshes
	terrainMesh = new TerrainMesh(renderer->getDevice(), renderer->getDeviceContext(), res);
	skyMesh = new Model(renderer->getDevice(), renderer->getDeviceContext(), "../res/mySphere.obj");
	//create control matrixs
	terrainMatrix =XMMatrixScaling(0.4,0.4,0.4) * XMMatrixTranslation(-10,-30,-10);
	skyMatrix = XMMatrixScaling(90, 60, 90)*XMMatrixTranslation(20,-20,30);
	//create shaders
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	terrainShader = new TerrainShader(renderer->getDevice(), hwnd);
	colourShader = new ColourShader(renderer->getDevice(), hwnd);
	//render targets
	depthMap = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	
	textureLoading();
	
	//set up lights
	mainLight = new Light;
	mainLight->setAmbientColour(0.4f, 0.5f, 0.4f, 1.0f);
	mainLight->setDiffuseColour(0.7f, 0.7f, 0.5f, 1.0f);
	mainLight->setDirection(0.f, -1.0f, 0.f);
	//info for shaddow mapping
	mainLight->setPosition(40.f, 100.0f, 40.f);
	mainLight->setLookAt(40.f, -20.0f, 40.f);
	mainLight->generateViewMatrix();
	mainLight->generateProjectionMatrix(1.0f, 100.0f);

	//controllers
	cameraController = new CameraControl(res);

}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();
	skyMatrix = XMMatrixTranslation(-camera->getPosition().x, -camera->getPosition().y, 0);
	// Release the Direct3D object.
	if (terrainMesh)
	{
		delete terrainMesh;
		terrainMesh = 0;
	}
	if (terrainShader)
	{
		delete terrainShader;
		terrainShader = 0;
	}
	if (cameraController)
	{
		delete cameraController;
		cameraController = 0;
	}
}

bool App1::frame()
{
	bool result;
	processInput();
	time += 0.025f;
	result = BaseApplication::frame();
	skyMatrix = XMMatrixTranslation(camera->getPosition().x, camera->getPosition().y-3, camera->getPosition().z);
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.2f, 0.4f, 1.0f);
	//renderer->setWireframeMode(true);
	
	//// Generate the view matrix based on the camera's position.
	camera->update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();
	renderer->setZBuffer(false);
	
	worldMatrix = skyMatrix;
	skyMesh->sendData(renderer->getDeviceContext());
	colourShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, skyTextures[0], time);
	colourShader->render(renderer->getDeviceContext(), skyMesh->getIndexCount());
	renderer->setZBuffer(true);
	//terrian
	worldMatrix = terrainMatrix;
	terrainMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	terrainShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, mainLight, terrainTextures, time);
	//// Render object (combination of mesh geometry and shader process
	terrainShader->render(renderer->getDeviceContext(), terrainMesh->getIndexCount());
	
	
	
	
	// Render GUI
	gui();

	//// Present the rendered scene to the screen.
	renderer->endScene();

	return true;
}

void App1::gui()
{
	// Force turn off on Geometry shader
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	// Render UI
	ImGui::Render();
}

void App1::processInput()
{
	if (input->isKeyDown(VK_SPACE))
	{
		terrainMesh->AddFaultLine(renderer->getDevice());
		terrainMesh->ApplyAreaMap(renderer->getDevice());
	}
	if (input->isKeyDown(VK_DELETE))
	{
		terrainMesh->RestartTerrain(renderer->getDevice());
	}
	if (input->isKeyDown(VK_CONTROL))
	{
		terrainMesh->smooth(renderer->getDevice());
	}
}

void App1::textureLoading()
{
	//// //// TERRIAN TEXTURES //// ////
	//load textures
	textureMgr->loadTexture("grass", L"../res/grass.png");
	textureMgr->loadTexture("rock1", L"../res/rock1.png");
	textureMgr->loadTexture("rock2", L"../res/rock2.png");
	textureMgr->loadTexture("water", L"../res/water.png");
	textureMgr->loadTexture("hillside", L"../res/hillside.png");
	textureMgr->loadTexture("cliff", L"../res/cliff.png");
	//set up texture array
	terrainTextures[0] = textureMgr->getTexture("grass");
	terrainTextures[1] = textureMgr->getTexture("rock1");
	terrainTextures[2] = textureMgr->getTexture("water");
	terrainTextures[3] = textureMgr->getTexture("hillside");
	terrainTextures[4] = textureMgr->getTexture("cliff");
	terrainTextures[5] = textureMgr->getTexture("rock2");

	//// //// SKYBOX TEXTURES //// ////
	patternGenerator = new PatternCreation(512);
	patternGenerator->CreateTexture(renderer->getDevice());
	skyTextures[0] = patternGenerator->getCloudTexture(renderer->getDevice());
	skyTextures[1] = patternGenerator->getDetailTexture(renderer->getDevice());

}

