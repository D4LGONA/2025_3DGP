#include "stdafx.h"
#include "TitleScene.h"

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
	ReleaseObjects();
}

bool TitleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (nMessageID == WM_KEYDOWN)
	{
		switch (wParam)
		{
		}
	}
	return false;
}

bool TitleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		if (nMessageID == WM_RBUTTONDOWN || nMessageID == WM_LBUTTONDOWN) {
			if (objects.back() == PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam), pPlayer->GetCamera())) {
				auto a = reinterpret_cast<CExplosiveObject*>(objects.back());
				if (!a->m_bBlowingUp)
				{
					a->StartExplosion(); // 폭발 시작
					isExplosive = true;
				}
			}
		}
	}
	return false;
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
	if (isExplosive && false == reinterpret_cast<CExplosiveObject*>(objects.back())->m_bBlowingUp) {
		change = true;
		idx = 1; // 폭발 후 씬 변경
	}

	if (pPlayer) pPlayer->Update(fTimeElapsed); // 플레이어(및 카메라) 업데이트
	for (auto& obj : objects)
	{
		if (obj) obj->Animate(fTimeElapsed); // 각 오브젝트 업데이트
	}
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
	auto nameobj = new CRotatingObject();
	nameobj->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f)); // Y축을 회전축으로 설정
	nameobj->SetRotationSpeed((((rand() / (float)RAND_MAX) * 10.0f) - 5.0f) * 10.0f);
	nameobj->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	nameobj->SetShader(pShader);
	nameobj->SetPosition(XMFLOAT3(0.0f, 10.0f, 50.0f));
	nameobj->Rotate(0.0f, 180.0f, 0.0f); // 180도 회전
	nameobj->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "3DGP.obj", XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)));
	objects.push_back(nameobj);


	auto obj = new CExplosiveObject(); // Rotating Object로 만든 후에 피킹이 일어나면 Explosive Object로 변경.
	obj->setExplosionMesh(new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f));
	obj->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	obj->SetExpShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	obj->SetShader(pShader);
	obj->SetPosition(XMFLOAT3(0.0f, -10.0f, 50.0f));
	obj->Rotate(0.0f, 180.0f, 0.0f); // 180도 회전
	obj->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "NAME.obj", XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	objects.push_back(obj);
}

void TitleScene::ReleaseObjects()
{
}

CGameObject* TitleScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera * pCamera)
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