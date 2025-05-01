#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "Player.h"
#include "Scene.h"


class CMenuScene : public CScene
{
	CGameObject* m_pTutorial;
	CGameObject* m_pLv1;
	CGameObject* m_pLv2;
	CGameObject* m_pStart;
	CGameObject* m_pEnd;

	CPlayer* m_pPlayer;

	CCamera* CreateCamera();
	bool	bChange = false;
	bool doChangeColor = false;

	CGameObject* curPickingObject = nullptr;

public:
	~CMenuScene() {}

	void BuildObjects();
	void ReleaseObjects();

	void Animate(float fElapsedTime);
	void Render(HDC hDCFrameBuffer);

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void ProcessInput(POINT oldCursorPos, HWND hWnd, float m_fElapsedTime);
	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
};

