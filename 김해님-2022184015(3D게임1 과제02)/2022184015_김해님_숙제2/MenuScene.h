#pragma once
#include "Scene.h"

class MenuScene : public CScene
{
public:
	MenuScene(CGameTimer* timer);
	virtual ~MenuScene();

	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice) override;
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void ReleaseObjects() override;
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;

	virtual void AnimateObjects(float fTimeElapsed) override;

public:
	//���� ��� ���� ��ü�鿡 ���� ���콺 ��ŷ�� �����Ѵ�.
	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
	virtual void Reset(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {};
private:
	CPlayer* pPlayer = nullptr;

	std::vector<CGameObject*> objects;

};

