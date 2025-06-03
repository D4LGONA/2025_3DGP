#include "stdafx.h"
#include "MenuScene.h"
#include "Player.h"

MenuScene::MenuScene()
{
}

MenuScene::~MenuScene()
{
	ReleaseObjects();
}

bool MenuScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool MenuScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		if (nMessageID == WM_RBUTTONDOWN || nMessageID == WM_LBUTTONDOWN) {
			auto obj = PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam), pPlayer->GetCamera());
			if (obj == nullptr)
				return false;
			else
			{
				if (obj == objects.back()) exit(0);
			}
		}
	}
	return false;
}

void MenuScene::CreateGraphicsPipelineState(ID3D12Device* pd3dDevice)
{
	if (!m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CScene::CreateGraphicsPipelineState(pd3dDevice); // 공통 파이프라인 사용
}

void MenuScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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
	auto m_pTutorial = new CGameObject();
	m_pTutorial->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pTutorial->SetShader(pShader);
	m_pTutorial->Rotate(0.0f, 180.0f, 0.0f); // 180도 회전
	m_pTutorial->SetPosition(-0.0f, 10.0f, 30.0f);
	m_pTutorial->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "Tutorial.obj", XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)));
	objects.push_back(m_pTutorial);

	auto m_pLv1 = new CGameObject();
	m_pLv1->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pLv1->SetShader(pShader);
	m_pLv1->Rotate(0.0f, 180.0f, 0.0f); // 180도 회전
	m_pLv1->SetPosition(-10.0f, 0.0f, 30.0f);
	m_pLv1->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "Lv1.obj", XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)));
	objects.push_back(m_pLv1);

	auto m_pLv2 = new CGameObject();
	m_pLv2->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pLv2->SetShader(pShader);
	m_pLv2->Rotate(0.0f, 180.0f, 0.0f); // 180도 회전
	m_pLv2->SetPosition(10.0f, 0.0f, 30.0f);
	m_pLv2->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "Lv2.obj", XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)));
	objects.push_back(m_pLv2);

	auto m_pStart = new CGameObject();
	m_pStart->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pStart->SetShader(pShader);
	m_pStart->Rotate(0.0f, 180.0f, 0.0f); // 180도 회전
	m_pStart->SetPosition(-10.0f, -10.0f, 30.0f);
	m_pStart->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "Start.obj", XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)));
	objects.push_back(m_pStart);

	auto m_pEnd = new CGameObject();
	m_pEnd->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pEnd->SetShader(pShader);
	m_pEnd->Rotate(0.0f, 180.0f, 0.0f); // 180도 회전
	m_pEnd->SetPosition(10.0f, -10.0f, 30.0f);
	m_pEnd->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "End.obj", XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)));
	objects.push_back(m_pEnd);
}

void MenuScene::ReleaseObjects()
{
}

void MenuScene::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 카메라가 없거나 오브젝트가 없다면 렌더링 생략
	if (!pPlayer || !pPlayer->GetCamera()) return;

	auto pCamera = pPlayer->GetCamera();

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	// 오브젝트 렌더링
	for (auto& obj : objects)
	{
		if (obj)
			obj->Render(pd3dCommandList, pPlayer->GetCamera());
	}

#ifdef _WITH_PLAYER_TOP
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif

	if (pPlayer) pPlayer->Render(pd3dCommandList);
}

ID3D12RootSignature* MenuScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	return CScene::CreateGraphicsRootSignature(pd3dDevice); // CScene 공통 루트 시그니처 사용
}

void MenuScene::AnimateObjects(float fTimeElapsed)
{
	if (pPlayer) pPlayer->Update(fTimeElapsed); // 플레이어(및 카메라) 업데이트
	for (auto& obj : objects)
	{
		if (obj) obj->Animate(fTimeElapsed); // 각 오브젝트 업데이트
	}
	CScene::AnimateObjects(fTimeElapsed);       // 공통 셰이더 객체들 업데이트
}

CGameObject* MenuScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	if (!pCamera) return(NULL);
	XMFLOAT4X4 xmf4x4View = pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = pCamera->GetProjectionMatrix();
	D3D12_VIEWPORT d3dViewport = pCamera->GetViewport();
	XMFLOAT3 xmf3PickPosition;
	/*화면 좌표계의 점 (xClient, yClient)를 화면 좌표 변환의 역변환과 투영 변환의 역변환을 한다. 그 결과는 카메라
   좌표계의 점이다. 투영 평면이 카메라에서 z-축으로 거리가 1이므로 z-좌표는 1로 설정한다.*/
	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;
	int nIntersected = 0;
	float fHitDistance = FLT_MAX, fNearestHitDistance = FLT_MAX;
	CGameObject* pIntersectedObject = NULL, * pNearestObject = NULL;
	//셰이더의 모든 게임 객체들에 대한 마우스 픽킹을 수행하여 카메라와 가장 가까운 게임 객체를 구한다.
	/*for (int i = 0; i < m_nShaders; i++)
	{
		pIntersectedObject = m_pShaders[i].PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, &fHitDistance);
		if (pIntersectedObject && (fHitDistance < fNearestHitDistance))
		{
			fNearestHitDistance = fHitDistance;
			pNearestObject = pIntersectedObject;
		}
	}*/

	for (auto& obj : objects)
	{
		if (obj)
		{
			nIntersected = obj->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, &fHitDistance);
			if (nIntersected > 0 && fHitDistance < fNearestHitDistance)
			{
				fNearestHitDistance = fHitDistance;
				pNearestObject = obj;
			}
		}
	}
	return(pNearestObject);
}
