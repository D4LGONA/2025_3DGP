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
			// ��� expobj�� ��Ʈ����.
			for (auto& obj : objects)
			{
				auto a = dynamic_cast<CExplosiveObject*>(obj);
				if (a == nullptr) continue; // CExplosiveObject�� �ƴ� ��� ����
				if (!a->m_bBlowingUp)
				{
					a->StartExplosion(); // ���� ����
				}
			}
			break;
		case VK_CONTROL:

			//m_pLockedObject = NULL;
			break;
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
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		if (nMessageID == WM_RBUTTONDOWN || nMessageID == WM_LBUTTONDOWN) {
			// todo: ���⿡ ��ŷ ���� �߰�
		}
	}
	return false;
}

void Scene2::CreateGraphicsPipelineState(ID3D12Device* pd3dDevice)
{
	if (!m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CScene::CreateGraphicsPipelineState(pd3dDevice); // ���� ���������� ���
}

void Scene2::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// ��Ʈ �ñ״�ó ����
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	auto mesh = new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "head.obj", XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

	// �÷��̾� ����
	pPlayer = new CPlayer();

	//�÷��̾ ���� ���̴� ������ �����Ѵ�. 
	pPlayer->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//�÷��̾��� ��ġ�� �����Ѵ�. 
	pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	//�÷��̾�(�����) �޽��� �������� �� ����� ���̴��� �����Ѵ�.
	CDiffusedShader* pShader = new CDiffusedShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pPlayer->SetShader(pShader);
	pPlayer->SetMesh(mesh); // �������� �Ӹ�.

	// ī�޶� ���� -> 3��Ī ī�޶�� ��������� ��.
	CCamera* pCamera = new CCamera();
	pPlayer->SetFriction(200.0f);
	pPlayer->SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
	pPlayer->SetMaxVelocityXZ(125.0f);
	pPlayer->SetMaxVelocityY(400.0f);
	pPlayer->SetCamera(pCamera);
	pPlayer->SetCamera(pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, m_SceneTimer->GetTimeElapsed()));

	pBody = new CGameObject();
	pBody->SetShader(pShader);
	pBody->SetMesh(new CCubeMeshDiffused(pd3dDevice, pd3dCommandList)); // �������� �Ӹ�.
	pBody->SetPosition(pPlayer->GetPosition().x, pPlayer->GetPosition().y - 1.0f, pPlayer->GetPosition().z);

	// ���޽� -> ȸ�� �������� ī�޶��� �ٶ󺸰� ����.
	// ������Ʈ ����
	for (int i = 0; i < 20; ++i) {
		auto obj = new CExplosiveObject(); // Rotating Object�� ���� �Ŀ� ��ŷ�� �Ͼ�� Explosive Object�� ����.
		obj->setExplosionMesh(new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f));
		obj->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		obj->SetExpShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
		obj->SetShader(pShader);
		obj->SetPosition(XMFLOAT3(0.0f, -0.0f, 10.0f));
		obj->Rotate(0.0f, 0.0f, 0.0f); 
		obj->SetMesh(mesh);
		objects.push_back(obj);
	}

}

void Scene2::ReleaseObjects()
{
}

void Scene2::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// ī�޶� ���ų� ������Ʈ�� ���ٸ� ������ ����
	if (!pPlayer || !pPlayer->GetCamera()) return;

	auto pCamera = pPlayer->GetCamera();

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);


	// ������Ʈ ������
	for (auto& obj : objects)
	{
		if (obj)
			obj->Render(pd3dCommandList, pPlayer->GetCamera());
	}


#ifdef _WITH_PLAYER_TOP
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif

	//3��Ī ī�޶��� �� �÷��̾ �������Ѵ�. 
	if (pPlayer) {
		pPlayer->Render(pd3dCommandList, pPlayer->GetCamera());
		pBody->Render(pd3dCommandList, pPlayer->GetCamera()); // �÷��̾� ��ü ������
	}
}

ID3D12RootSignature* Scene2::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	return CScene::CreateGraphicsRootSignature(pd3dDevice); // CScene ���� ��Ʈ �ñ״�ó ���
}

void Scene2::AnimateObjects(float fTimeElapsed)
{
	if (pPlayer) pPlayer->Update(fTimeElapsed); // �÷��̾�(�� ī�޶�) ������Ʈ
	for (auto& obj : objects)
	{
		if (obj) obj->Animate(fTimeElapsed); // �� ������Ʈ ������Ʈ
	}
	CScene::AnimateObjects(fTimeElapsed);       // ���� ���̴� ��ü�� ������Ʈ
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
		pBody->SetPosition(pPlayer->GetPosition().x, pPlayer->GetPosition().y - 1.0f, pPlayer->GetPosition().z); // �÷��̾� ��ü ��ġ ������Ʈ
	}
}
