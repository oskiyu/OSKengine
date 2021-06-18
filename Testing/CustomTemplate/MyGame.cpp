#include "MyGame.h"

void MyGame::OnCreate() {
	windowCreateInfo.Name = "MyGame";
	windowCreateInfo.SizeX = 800;
	windowCreateInfo.SizeY = 480;

	rendererCreateInfo.GameName = "MyGame";
	rendererCreateInfo.GameVersion = OSK::Version{ 0, 0, 0 };
}

void MyGame::LoadContent() {
	renderSystem3D->GetRenderScene()->LoadSkybox("skybox/sky");
}

void MyGame::OnTick(deltaTime_t delta) {
	
}

void MyGame::OnDraw2D() {

}

void MyGame::OnExit() {

}

