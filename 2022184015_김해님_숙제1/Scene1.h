#pragma once
#include "Scene.h"
#include "GameObject.h"

class CScene_1 : public CScene
{
public:
	CScene_1();
	virtual ~CScene_1();

private:

	int LoadLineToMesh(const char* filename, std::vector<XMFLOAT3>& vertices, std::vector< std::array<int, 2>>& edges);
	void BuildOrderedLinePath(const std::vector<XMFLOAT3>& vertices, const std::vector<std::array<int, 2>>& edges, std::vector<XMFLOAT3>& outPath);
	
	CPlayer* m_pCart = NULL;

	CPlayer* m_pPlayer = NULL;
	CPlayer* m_pDummy = NULL;
	CGameObject* m_pTrack = nullptr;
	CGameObject* m_pdummy = nullptr;
	CCamera* CreateCamera();

	std::vector<XMFLOAT3> m_vTrackPoints;
	std::vector<std::array<int, 2>> m_vTrackEdges;

	int m_iCurrentPathIndex = 1;
	float m_fMoveSpeed = 20.f;
	float m_fCurrentSpeed = 20.0f; // 초기 속도
	float timer = 0.0f;
	
	float m_fTargetYaw = 0.0f;
	float m_fTargetPitch = 0.0f;
	float m_fTargetRoll = 0.0f;
	
	float m_fCurrentYaw = 0.0f;
	float m_fCurrentPitch = 0.0f;
	float m_fCurrentRoll = 0.0f;	

#ifdef _WITH_DRAW_AXIS
	CGameObject* m_pWorldAxis = NULL;
#endif

public:
	virtual void BuildObjects();
	virtual void ReleaseObjects();
	virtual void ResetObjects();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);

	virtual void ProcessInput(POINT oldCursorPos, HWND hWnd, float m_fElapsedTime);
};

