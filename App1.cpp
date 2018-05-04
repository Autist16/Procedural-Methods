// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	//BaseApplication::BaseApplication();
	terrainShader = nullptr;
	skyShader = nullptr;
	postShader = nullptr;

	terrainMesh = nullptr;
	skyMesh = nullptr;
	orthoMesh = nullptr;

	postProcessTex = nullptr;

	zoom = nullptr;
	wire = nullptr;
	patternGenerator = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);

	//set up inital camera positon
	camera->setPosition(6, -4, 6);
	camBounds = XMFLOAT2(6, 20);
	// Create Meshes
	terrainMesh = new TerrainMesh(renderer->getDevice(), renderer->getDeviceContext(), res);
	skyMesh = new Model(renderer->getDevice(), renderer->getDeviceContext(), "../res/mySphere.obj");
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight,0,0);
	
	//create control matrixs
	terrainMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f)*XMMatrixTranslation(-10,-16,-10);
	
	//create shaders
	terrainShader = new TerrainShader(renderer->getDevice(), hwnd);
	skyShader = new SkyShader(renderer->getDevice(), hwnd);
	postShader = new PostProcessingShader(renderer->getDevice(), hwnd);

	//render targets
	postProcessTex = new RenderTexture(renderer->getDevice(), screenWidth*2, screenHeight*2, SCREEN_NEAR, SCREEN_DEPTH);
	
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
	zoom = new bool(false);
	wire = new bool(false);
	scnHeight = screenHeight;
	scnWidth = screenWidth;
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();
	// Release the Direct3D object.
	if (terrainMesh)
	{
		delete terrainMesh;
		terrainMesh = 0;
	}
	if (skyMesh)
	{
		delete skyMesh;
		skyMesh = 0;
	}
	if (terrainShader)
	{
		delete terrainShader;
		terrainShader = 0;
	}
	if (skyShader)
	{
		delete skyShader;
		skyShader = 0;
	}
	if (postShader)
	{
		delete postShader;
		postShader = 0;
	}

}

bool App1::frame()
{
	bool result;
	processInput();
	time += 0.025f;
	result = BaseApplication::frame();
	skyMatrix = XMMatrixScaling(0.3f,0.25f,0.3f) * XMMatrixTranslation(camera->getPosition().x, camera->getPosition().y-0.65, camera->getPosition().z);
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
	renderToTexture();
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	//// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.2f, 0.4f, 1.0f);
	
	//// Generate the view matrix based on the camera's position.
	camera->update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();
	
	//sky box render
	renderer->setZBuffer(false);
	worldMatrix = skyMatrix;
	skyMesh->sendData(renderer->getDeviceContext());
	skyShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, skyTextures, time);
	skyShader->render(renderer->getDeviceContext(), skyMesh->getIndexCount());
	renderer->setZBuffer(true);
	
	//terrian
	terrainShader->createRasterState(renderer->getDevice(), *wire);
	worldMatrix = terrainMatrix;
	terrainMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	terrainShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, mainLight, terrainTextures, time);
	//// Render object (combination of mesh geometry and shader process
	terrainShader->render(renderer->getDeviceContext(), terrainMesh->getIndexCount());
	
	//reset world matrix
	worldMatrix = renderer->getWorldMatrix();
	
	if (*zoom == true)
	{
		renderer->setZBuffer(false);
		//render orthomesh
		orthoMatrix = renderer->getOrthoMatrix();
		orthoViewMatrix = camera->getOrthoViewMatrix();

		orthoMesh->sendData(renderer->getDeviceContext());
		postShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, postProcessTex->getShaderResourceView(), scnWidth, scnHeight);
		postShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

		renderer->setZBuffer(true);
	}
	// Render GUI
	gui();

	//// Present the rendered scene to the screen.
	renderer->endScene();

	return true;
}

void App1::renderToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	//set render target to render to texture
	postProcessTex->setRenderTarget(renderer->getDeviceContext());
	//clear render to texture
	postProcessTex->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.1f, 0.1f, 1.0f);
	//generate vewi matrix from camera pos
	camera->update();

	//get matrixs from camera and objects
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();
	
	//sky box render
	renderer->setZBuffer(false);
	worldMatrix = skyMatrix;
	skyMesh->sendData(renderer->getDeviceContext());
	skyShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, skyTextures, time);
	skyShader->render(renderer->getDeviceContext(), skyMesh->getIndexCount());
	renderer->setZBuffer(true);
	
	//terrian
	worldMatrix = terrainMatrix;
	terrainMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	terrainShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, mainLight, terrainTextures, time);
	//// Render object (combination of mesh geometry and shader process
	terrainShader->render(renderer->getDeviceContext(), terrainMesh->getIndexCount());

	//reset world matrix
	worldMatrix = renderer->getWorldMatrix();
	//reset the render target to back buffer
	renderer->setBackBufferRenderTarget();
}
void App1::gui()
{
	// Force turn off on Geometry shader
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Zoom", zoom);
	ImGui::Checkbox("Wireframe Mode", wire);
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
	textureMgr->loadTexture("water", L"../res/water.png");
	textureMgr->loadTexture("standard", L"../res/hillside.png");
	textureMgr->loadTexture("snow", L"../res/snow.png");

	//set up texture array
	terrainTextures[0] = textureMgr->getTexture("grass");
	terrainTextures[1] = textureMgr->getTexture("rock1");
	terrainTextures[2] = textureMgr->getTexture("water");
	terrainTextures[3] = textureMgr->getTexture("standard");
	terrainTextures[4] = textureMgr->getTexture("snow");

	//// //// SKYBOX TEXTURES //// ////
	patternGenerator = new PatternCreation(1024);
	patternGenerator->CreateTexture(renderer->getDevice());
	skyTextures[0] = patternGenerator->getCloudTexture(renderer->getDevice());
	skyTextures[1] = patternGenerator->getDetailTexture(renderer->getDevice());

}

