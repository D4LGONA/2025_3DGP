#pragma once
#include "Scene.h"
#include "Player.h"

// ��ũ����.

// �ϴ� ���� ������ �׽�Ʈ���� �� �غ��� ��
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
	//���� ��� ���� ��ü�鿡 ���� ���콺 ��ŷ�� �����Ѵ�.
	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
private:
	CPlayer* pPlayer = nullptr;
	CGameObject* pBody = nullptr; // ��ũ ��ü

	bool AutoTarget = false;

	CGameObject* pickedObj = nullptr;

	CGameObject* map = nullptr; 
	std::vector<CGameObject*> objects;
};

