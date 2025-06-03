#include "stdafx.h"
#include "TitleScene.h"

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
	ReleaseObjects();
}

void TitleScene::CreateGraphicsPipelineState(ID3D12Device* pd3dDevice)
{
	if (!m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CScene::CreateGraphicsPipelineState(pd3dDevice); // 공통 파이프라인 사용
}

void TitleScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	// 카메라가 없거나 오브젝트가 없다면 렌더링 생략
	if (!pPlayer || !pPlayer->GetCamera()) return;

	// 카메라 셰이더 상수 업데이트
	pPlayer->GetCamera()->UpdateShaderVariables(pd3dCommandList);

	// 플레이어 셰이더 상수 업데이트 (예: 월드 행렬 등)
	pPlayer->UpdateShaderVariables(pd3dCommandList);

	// 오브젝트 렌더링
	for (auto& obj : objects)
	{
		if (obj) obj->Render(pd3dCommandList, pPlayer->GetCamera());
	}
}

ID3D12RootSignature* TitleScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	return CScene::CreateGraphicsRootSignature(pd3dDevice); // CScene 공통 루트 시그니처 사용
}

void TitleScene::AnimateObjects(float fTimeElapsed)
{
	if (pPlayer) pPlayer->Update(fTimeElapsed); // 플레이어(및 카메라) 업데이트
	CScene::AnimateObjects(fTimeElapsed);       // 공통 셰이더 객체들 업데이트
}

void TitleScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 루트 시그니처 생성
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	// 플레이어 생성
	//pPlayer = new CPlayer();

	////플레이어를 위한 셰이더 변수를 생성한다. 
	//pPlayer->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	////플레이어의 위치를 설정한다. 
	//pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	////플레이어(비행기) 메쉬를 렌더링할 때 사용할 셰이더를 생성한다.
	//CObjectsShader* pShader = new CObjectsShader();
	//pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	//pPlayer->SetShader(pShader);

	//// 카메라 설정
	//CCamera* pCamera = new CCamera();
	//pPlayer->SetFriction(200.0f);
	//pPlayer->SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
	//pPlayer->SetMaxVelocityXZ(125.0f);
	//pPlayer->SetMaxVelocityY(400.0f);
	//pCamera->SetTimeLag(0.0f);
	//pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
	//pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
	//pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	//pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	//pPlayer->SetCamera(pCamera);


	// 타이틀용 셰이더 생성
	m_pTitleShader = new CInstancingShader();
	m_pTitleShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pTitleShader->BuildObjects(pd3dDevice, pd3dCommandList);

	// 오브젝트 빌드
	// 오브젝트 0: 3D프로그래밍1 메쉬


	m_pShaders.push_back(m_pTitleShader);
}

void TitleScene::ReleaseObjects()
{
}
