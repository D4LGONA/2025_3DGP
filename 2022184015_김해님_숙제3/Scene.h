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

	//������ ���콺�� Ű���� �޽����� ó���Ѵ�
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice);
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void ReleaseUploadBuffers();
	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

	ID3D12PipelineState *m_pd3dPipelineState = NULL;
	//���������� ���¸� ��Ÿ���� �������̽� �������̴�. 

public: // �浹
	void CheckEnemyByBulletCollisions();
	void CheckEnemyByPlayerCollisions();
	void CheckEnemyByEnemyCollisions();
	void CheckEnemyNearbyPlayer();
protected:
	CHeightMapTerrain* m_pTerrain = NULL;
public:
	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }

protected:
	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 
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