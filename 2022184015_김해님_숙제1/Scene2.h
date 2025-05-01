#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "Player.h"
#include "Scene.h"
#include "EnemyTank.h"

class CScene_2 : public CScene // tank game scene
{
public:
	CScene_2();
	virtual ~CScene_2();

private:

	int							m_nObjects = 0;
	CGameObject					**m_ppObjects = NULL; // 장애물
	int							m_nEnemies = 0;
	CEnemyTank					** m_pEnemyTanks = nullptr; // 적들
	int							m_remainCount = 10;

	CWallsObject*				m_pWallsObject = NULL;

	CTankPlayer*				m_pPlayer = NULL;
	CGameObject*				m_PlayerBody = NULL;

	CGameObject*				m_pLockedObject = NULL;

	CGameObject* m_pShield = nullptr;

	CGameObject* m_YouWinObject = nullptr;

	CCamera* CreateCamera();
	void BuildEnemies();
	void BuildObstacles();

	bool m_bAutoAttack = false;  
	bool m_bShieldOn = false;    
	float m_fShieldTimer = 0.0f;

#ifdef _WITH_DRAW_AXIS
	CGameObject*				m_pWorldAxis = NULL;
#endif

public:
	virtual void BuildObjects();
	virtual void ReleaseObjects();

	void ResetObjects();

	void CheckObjectByObjectCollisions();
	void CheckObjectByWallCollisions();
	void CheckPlayerByWallCollision();
	void CheckObjectByBulletCollisions();
	void CheckEnemyByBulletCollisions();
	void CheckEnemyTankCollisions();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);

	virtual void ProcessInput(POINT oldCursorPos, HWND hWnd, float m_fElapsedTime);
};

