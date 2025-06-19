#pragma once
#include "Timer.h"
#include "Shader.h"
#include "Camera.h"
#include "Player.h"

class CScene
{
public:
	CScene();
	~CScene() {};

	//씬에서 마우스와 키보드 메시지를 처리한다
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice);
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void ReleaseUploadBuffers();
	//그래픽 루트 시그너쳐를 생성한다. 
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

	ID3D12PipelineState *m_pd3dPipelineState = NULL;
	//파이프라인 상태를 나타내는 인터페이스 포인터이다. 

public: // 충돌
	void CheckEnemyByBulletCollisions();
	void CheckEnemyByPlayerCollisions();
	void CheckEnemyByEnemyCollisions();
	void CheckEnemyNearbyPlayer();
protected:
	CHeightMapTerrain* m_pTerrain = NULL;
public:
	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }

protected:
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다. 
	CObjectsShader *m_pShaders = NULL;
	int m_nShaders = 0;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	std::vector<CTankObject*> enemies;
public:
	int enemycount = 10;
	bool bHit = false;
	CTerrainPlayer* Player = nullptr;

	float delay = 0.0f;

	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
	void ExplosiveAllEnemies()
	{
		for (auto& pEnemy : enemies) {
			if (pEnemy->m_bBlowingUp) continue;
			pEnemy->StartExplosion();
		}
		enemycount = 0;
	}
};