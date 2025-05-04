#pragma once

#include "Scene.h"
#include "GameObject.h"

class CTitleScene : public CScene
{
	CGameObject* m_pTitle;
	CGameObject* m_pName;

	CPlayer* m_pPlayer;

	CCamera* CreateCamera();
	bool	bChange = false;

public:
	CTitleScene() {};
	virtual ~CTitleScene() {}

	virtual void BuildObjects();
	virtual void ReleaseObjects();
	void ResetObjects();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void ProcessInput(POINT oldCursorPos, HWND hWnd, float m_fElapsedTime);
	bool PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
};

