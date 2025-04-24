#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "Player.h"
#include "Scene.h"

class CScene_2 : public CScene // tank game scene
{
public:
	CScene_2();
	virtual ~CScene_2();

private:

	int							m_nObjects = 0;
	CGameObject					**m_ppObjects = NULL;
	CWallsObject*				m_pWallsObject = NULL;
	CTankPlayer*					m_pPlayer = NULL;

	CCamera* CreateCamera();
	void BuildEnemies();

	bool m_bAutoAttack = false;  
	bool m_bShieldOn = false;    

#ifdef _WITH_DRAW_AXIS
	CGameObject*				m_pWorldAxis = NULL;
#endif

public:
	virtual void BuildObjects();
	virtual void ReleaseObjects();

	void CheckObjectByObjectCollisions();
	void CheckObjectByWallCollisions();
	void CheckPlayerByWallCollision();
	void CheckObjectByBulletCollisions();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);

	virtual void ProcessInput(POINT oldCursorPos, HWND hWnd, float m_fElapsedTime);
};

