#pragma once

#include "stdafx.h"
#include "Scene.h"
#include "player.h"

// Title Scene: 이름과 3D프로그래밍1 메쉬를 띄우는 씬
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
	//씬의 모든 게임 객체들에 대한 마우스 픽킹을 수행한다.
	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
private:
	CPlayer* pPlayer = nullptr;
	bool isExplosive = false; // 폭발 여부

	std::vector<CGameObject*> objects;
};