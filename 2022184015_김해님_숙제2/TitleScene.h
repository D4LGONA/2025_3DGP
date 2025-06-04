#pragma once

#include "stdafx.h"
#include "Scene.h"
#include "player.h"

// Title Scene: �̸��� 3D���α׷���1 �޽��� ���� ��
class TitleScene : public CScene
{
public:
	TitleScene(CGameTimer* timer);
	virtual ~TitleScene();

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
private:
	CPlayer* pPlayer = nullptr;
	bool isExplosive = false; // ���� ����

	std::vector<CGameObject*> objects;
};