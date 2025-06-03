#pragma once

#include "stdafx.h"
#include "Scene.h"
#include "player.h"

// Title Scene: �̸��� 3D���α׷���1 �޽��� ���� ��
class TitleScene : public CScene
{
public:
	TitleScene();
	virtual ~TitleScene();

	virtual void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice) override;
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void ReleaseObjects() override;
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;

	virtual void AnimateObjects(float fTimeElapsed) override;
private:
	CPlayer* pPlayer = nullptr;
	CInstancingShader* m_pTitleShader = nullptr;

	std::vector<CGameObject*> objects;
};