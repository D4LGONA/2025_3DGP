#include "stdafx.h"
#include "Scene2.h"
#include "Player.h"

Scene2::Scene2(CGameTimer* timer)
	: CScene(timer)
{
}

Scene2::~Scene2()
{
	ReleaseObjects();
}

bool Scene2::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (nMessageID == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case 'a':
		case 'A':
			AutoTarget = !AutoTarget;
			break;
		case 'w':
		case 'W':
			// 모든 expobj를 터트린다.
			for (auto& obj : objects)
			{
				auto a = dynamic_cast<CExplosiveObject*>(obj);
				if (a == nullptr) continue; // CExplosiveObject가 아닌 경우 무시
				if (!a->m_bBlowingUp)
				{
					a->StartExplosion(); // 폭발 시작
				}
			}
			break;
		case VK_CONTROL:
		{
			auto pl = dynamic_cast<CTankPlayer*>(pPlayer);
				// 플레이어가 선택한 오브젝트가 없으면 아무것도 안함.
			pl->FireBullet(pickedObj);
			pickedObj = nullptr;
			break;
		}
		}
	}
	else if (nMessageID == WM_KEYUP)
	{
		switch (wParam)
		{
		case VK_F1:
		case VK_F2:
		case VK_F3:
			if (pPlayer) pPlayer->SetCamera(pPlayer->ChangeCamera((wParam - VK_F1 + 1), m_SceneTimer->GetTimeElapsed()));
			break;
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		}
		return false;
	}
}

bool Scene2::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
		pickedObj = PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam), pPlayer->GetCamera());
	}
	return false;
}

void Scene2::CreateGraphicsPipelineState(ID3D12Device* pd3dDevice)
{
	if (!m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CScene::CreateGraphicsPipelineState(pd3dDevice); // 공통 파이프라인 사용
}

void Scene2::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 루트 시그니처 생성
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	auto mesh = new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "head.obj", XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

	// 플레이어 생성
	pPlayer = new CTankPlayer(pd3dDevice, pd3dCommandList);

	//플레이어를 위한 셰이더 변수를 생성한다. 
	pPlayer->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//플레이어의 위치를 설정한다. 
	pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	//플레이어(비행기) 메쉬를 렌더링할 때 사용할 셰이더를 생성한다.
	CDiffusedShader* pShader = new CDiffusedShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pPlayer->SetShader(pShader);
	pPlayer->SetMesh(mesh); // 포신으로 머리.

	// 카메라 설정 -> 3인칭 카메라로 변경해줘야 함.
	CCamera* pCamera = new CCamera();
	pPlayer->SetFriction(200.0f);
	pPlayer->SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
	pPlayer->SetMaxVelocityXZ(125.0f);
	pPlayer->SetMaxVelocityY(400.0f);
	pPlayer->SetCamera(pCamera);
	pPlayer->SetCamera(pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, m_SceneTimer->GetTimeElapsed()));

	pBody = new CGameObject();
	pBody->SetShader(pShader);
	pBody->SetMesh(new CCubeMeshDiffused(pd3dDevice, pd3dCommandList)); // 포신으로 머리.
	pBody->SetPosition(pPlayer->GetPosition().x, pPlayer->GetPosition().y - 1.0f, pPlayer->GetPosition().z);

	// 헤드메쉬 -> 회전 안했을때 카메라쪽 바라보고 있음.
	// 오브젝트 빌드
	for (int i = 0; i < 20; ++i) {
		auto obj = new CTankObject(); // Rotating Object로 만든 후에 피킹이 일어나면 Explosive Object로 변경.
		obj->setExplosionMesh(new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f));
		obj->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		obj->SetExpShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
		obj->SetShader(pShader);
		obj->SetPosition(XMFLOAT3(RandF(-5.0f, 5.0f), 0.0f, RandF(-5.0f, 5.0f) + 10.0f));
		obj->SetMesh(mesh);
		obj->SetBody(new CCubeMeshDiffused(pd3dDevice, pd3dCommandList));
		objects.push_back(obj);
	}

	map = new CGameObject();
	map->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	map->SetShader(pShader);
	map->SetPosition(0.0f, 6.0f, 0.0f);
	map->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "map.obj", XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f)));
}

void Scene2::ReleaseObjects()
{
}

void Scene2::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 카메라가 없거나 오브젝트가 없다면 렌더링 생략
	if (!pPlayer || !pPlayer->GetCamera()) return;

	auto pCamera = pPlayer->GetCamera();

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);


	map->Render(pd3dCommandList, pCamera); // 맵 렌더링

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

	//3인칭 카메라일 때 플레이어를 렌더링한다. 
	if (pPlayer) {
		pPlayer->Render(pd3dCommandList, pPlayer->GetCamera());
		pBody->Render(pd3dCommandList, pPlayer->GetCamera()); // 플레이어 몸체 렌더링
	}
}

ID3D12RootSignature* Scene2::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	return CScene::CreateGraphicsRootSignature(pd3dDevice); // CScene 공통 루트 시그니처 사용
}

void Scene2::AnimateObjects(float fTimeElapsed)
{
	auto pl = dynamic_cast<CTankPlayer*>(pPlayer);
	if (pl) pl->Animate(fTimeElapsed); // 플레이어(및 카메라) 업데이트
	if (pl) pl->Update(fTimeElapsed); // 플레이어(및 카메라) 업데이트
	for (auto& obj : objects)
	{
		if (obj) obj->Animate(fTimeElapsed); // 각 오브젝트 업데이트
	}
	CScene::AnimateObjects(fTimeElapsed);       // 공통 셰이더 객체들 업데이트

	// 충돌체크
	CheckEnemyByBulletCollisions();
}

CGameObject* Scene2::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	if (!pCamera) return(NULL);
	XMFLOAT4X4 xmf4x4View = pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = pCamera->GetProjectionMatrix();
	D3D12_VIEWPORT d3dViewport = pCamera->GetViewport();
	XMFLOAT3 xmf3PickPosition;

	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;
	int nIntersected = 0;
	float fHitDistance = FLT_MAX, fNearestHitDistance = FLT_MAX;
	CGameObject* pIntersectedObject = NULL, * pNearestObject = NULL;

	for (auto& object : objects)
	{
		auto obj = dynamic_cast<CTankObject*>(object);
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

void Scene2::ProcessInput(const UCHAR* pKeyBuffer, float cxDelta, float cyDelta, float timeElapsed)
{
	if (pPlayer)
	{
		DWORD dwDirection = 0;
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;

		if (cxDelta || cyDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}

		if (dwDirection) pPlayer->Move(dwDirection, 500.0f * timeElapsed, true);
		pPlayer->Update(timeElapsed);
		pBody->SetPosition(pPlayer->GetPosition().x, pPlayer->GetPosition().y - 1.0f, pPlayer->GetPosition().z); // 플레이어 몸체 위치 업데이트
		ClampPlayerBodyPosition();
	}
}


void Scene2::CheckEnemyByBulletCollisions()
{
	CBulletObject** ppBullets = ((CTankPlayer*)pPlayer)->m_ppBullets;
	for (int i = 0; i < objects.size(); i++)
	{
		for (int j = 0; j < BULLETS; j++)
		{
			if (ppBullets[j]->bActive && objects[i]->GetBoundingBox().Intersects(ppBullets[j]->GetBoundingBox()))
			{
				CExplosiveObject* pExplosiveObject = (CExplosiveObject*)objects[i];
				if (pExplosiveObject->m_bBlowingUp == true) continue;
				pExplosiveObject->StartExplosion();
				ppBullets[j]->Reset();
			}
		}
	}
}

void Scene2::ClampPlayerBodyPosition()
{
	if (!pBody || !pPlayer) return;

	XMFLOAT3 bodyPos = pBody->GetPosition();

	// XZ 경계 제한
	bodyPos.x = std::clamp(bodyPos.x, -7.0f, 7.0f);

	// 위치 보정
	pBody->SetPosition(bodyPos);

	// pPlayer도 같이 이동 (머리 위치 = 몸체 위치 + 1.0f)
	pPlayer->SetPosition(XMFLOAT3(bodyPos.x, bodyPos.y + 1.0f, bodyPos.z));

	map->SetPosition(0.0f, 6.0f, pPlayer->GetPosition().z);
}