#pragma once
#include "Timer.h"
#include "Shader.h"
#include "Camera.h"

// CScene: 모든 씬 클래스의 부모 클래스이다.
class CScene
{
public:
	CScene();
	~CScene() {};

	//씬에서 마우스와 키보드 메시지를 처리
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);


	// 초기화 및 해제
	virtual void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice) = 0;
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) = 0;
	virtual void ReleaseObjects() = 0;

	// 프레임 업데이트
	virtual bool ProcessInput(UCHAR* pKeysBuffer);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList) = 0;
	virtual void ReleaseUploadBuffers();

	// 루트 시그니처
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) = 0;
	ID3D12RootSignature* GetGraphicsRootSignature() const { return m_pd3dGraphicsRootSignature; }

public:
	//씬의 모든 게임 객체들에 대한 마우스 픽킹을 수행한다.
	virtual CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera) = 0;

protected:
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다. 
	// 벡터를 사용함
	std::vector<CInstancingShader*> m_pShaders;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	ID3D12PipelineState *m_pd3dPipelineState = NULL;
};