#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "Player.h"
#include "Scene.h"

class CMenuScene : public CScene
{
	~CMenuScene() {}

	void BuildObjects() {};
	void ReleaseObjects() {};

	void CheckObjectByObjectCollisions() {};
	void CheckObjectByWallCollisions() {};
	void CheckPlayerByWallCollision() {};
	void CheckObjectByBulletCollisions() {};

	void Animate(float fElapsedTime) {};
	void Render(HDC hDCFrameBuffer) {};

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
	void ProcessInput(POINT oldCursorPos, HWND hWnd, float m_fElapsedTime);
};

