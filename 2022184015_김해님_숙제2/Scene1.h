#pragma once
#include "Scene.h"

class Scene1 :  public CScene
{
public:
	Scene1(CGameTimer* timer);
	virtual ~Scene1();

private:

	int LoadLineToMesh(const char* filename, std::vector<XMFLOAT3>& vertices, std::vector< std::array<int, 2>>& edges);
	void BuildOrderedLinePath(const std::vector<XMFLOAT3>& vertices, const std::vector<std::array<int, 2>>& edges, std::vector<XMFLOAT3>& outPath);

	CPlayer* m_pPlayer = NULL;

	CGameObject* m_pTrack = nullptr;

	std::vector<XMFLOAT3> m_vTrackPoints;
	std::vector<std::array<int, 2>> m_vTrackEdges;

	int m_iCurrentPathIndex = 1;
	float m_fMoveSpeed = 20.0f;
	float m_fCurrentSpeed = 20.0f;
	float timer = 0.0f;

public:
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice) override;
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void ReleaseObjects() override;
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;

	virtual void AnimateObjects(float fTimeElapsed) override;

	void ProcessInput(const UCHAR* pKeyBuffer, float cxDelta, float cyDelta, float timeElapsed) override;
	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera) { return nullptr; };
};

