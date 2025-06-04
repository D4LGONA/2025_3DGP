#pragma once
#include "Scene.h"
#include "Player.h"

// 탱크게임.

// 일단 폭발 여러개 테스트부터 좀 해볼까 함
class Scene2 : public CScene
{
public:
	Scene2(CGameTimer* timer);
	virtual ~Scene2();

	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice) override;
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void ReleaseObjects() override;
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;

	virtual void AnimateObjects(float fTimeElapsed) override;

	void ProcessInput(const UCHAR* pKeyBuffer, float cxDelta, float cyDelta, float timeElapsed) override;
	void CheckEnemyByBulletCollisions();
	void ClampPlayerBodyPosition();
public:
	//씬의 모든 게임 객체들에 대한 마우스 픽킹을 수행한다.
	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
private:
	CPlayer* pPlayer = nullptr;
	CGameObject* pBody = nullptr; // 탱크 객체

	bool AutoTarget = false;

	CGameObject* pickedObj = nullptr;

	CGameObject* map = nullptr; 
	std::vector<CGameObject*> objects;
};

