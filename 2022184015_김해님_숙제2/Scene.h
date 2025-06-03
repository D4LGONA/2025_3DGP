#pragma once
#include "Timer.h"
#include "Shader.h"
#include "Camera.h"

// CScene: ��� �� Ŭ������ �θ� Ŭ�����̴�.
class CScene
{
public:
	CScene();
	~CScene() {};

	//������ ���콺�� Ű���� �޽����� ó��
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);


	// �ʱ�ȭ �� ����
	virtual void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice) = 0;
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) = 0;
	virtual void ReleaseObjects() = 0;

	// ������ ������Ʈ
	virtual bool ProcessInput(UCHAR* pKeysBuffer);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList) = 0;
	virtual void ReleaseUploadBuffers();

	// ��Ʈ �ñ״�ó
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) = 0;
	ID3D12RootSignature* GetGraphicsRootSignature() const { return m_pd3dGraphicsRootSignature; }

public:
	//���� ��� ���� ��ü�鿡 ���� ���콺 ��ŷ�� �����Ѵ�.
	virtual CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera) = 0;

protected:
	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 
	// ���͸� �����
	std::vector<CInstancingShader*> m_pShaders;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	ID3D12PipelineState *m_pd3dPipelineState = NULL;
};