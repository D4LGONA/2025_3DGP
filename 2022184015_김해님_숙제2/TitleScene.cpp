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

void TitleScene::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 카메라가 없거나 오브젝트가 없다면 렌더링 생략
	if (!pPlayer || !pPlayer->GetCamera()) return;

	auto pCamera = pPlayer->GetCamera();

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	//// 플레이어 셰이더 상수 업데이트 (예: 월드 행렬 등)
	//pPlayer->UpdateShaderVariables(pd3dCommandList);

	// 오브젝트 렌더링
	for (auto& obj : objects)
	{
		if (obj) 
			obj->Render(pd3dCommandList, pPlayer->GetCamera());
	}


	//3인칭 카메라일 때 플레이어가 항상 보이도록 렌더링한다. 
#ifdef _WITH_PLAYER_TOP
	//렌더 타겟은 그대로 두고 깊이 버퍼를 1.0으로 지우고 플레이어를 렌더링하면 플레이어는 무조건 그려질 것이다. 
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif

	//3인칭 카메라일 때 플레이어를 렌더링한다. 
	if (pPlayer) pPlayer->Render(pd3dCommandList);
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

// 쉐이더 -> 모든 오브젝트에 대해 DiffusedShader 사용
void TitleScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 루트 시그니처 생성
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	// 플레이어 생성
	pPlayer = new CPlayer();

	//플레이어를 위한 셰이더 변수를 생성한다. 
	pPlayer->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//플레이어의 위치를 설정한다. 
	pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	//플레이어(비행기) 메쉬를 렌더링할 때 사용할 셰이더를 생성한다.
	CDiffusedShader* pShader = new CDiffusedShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pPlayer->SetShader(pShader);

	// 카메라 설정
	CCamera* pCamera = new CCamera();
	pPlayer->SetFriction(200.0f);
	pPlayer->SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
	pPlayer->SetMaxVelocityXZ(125.0f);
	pPlayer->SetMaxVelocityY(400.0f);
	pCamera->SetTimeLag(0.0f);
	pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
	pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
	pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	pPlayer->SetCamera(pCamera);


	// 오브젝트 빌드
	// 오브젝트 0: 3D프로그래밍1 메쉬
	objects.push_back(new CGameObject());
	objects[0]->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	objects[0]->SetShader(pShader);
	objects[0]->SetPosition(XMFLOAT3(0.0f, 10.0f, 50.0f));
	objects[0]->Rotate(0.0f, 180.0f, 0.0f); // 180도 회전
	objects[0]->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "3DGP.obj", XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)));

	objects.push_back(new CGameObject());
	objects[1]->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	objects[1]->SetShader(pShader);
	objects[1]->SetPosition(XMFLOAT3(0.0f, -10.0f, 50.0f));
	objects[1]->Rotate(0.0f, 180.0f, 0.0f); // 180도 회전
	objects[1]->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "NAME.obj", XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
}

void TitleScene::ReleaseObjects()
{
}
