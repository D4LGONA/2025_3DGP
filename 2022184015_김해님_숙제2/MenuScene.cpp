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

	CScene::CreateGraphicsPipelineState(pd3dDevice); // ���� ���������� ���
}

void MenuScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// ��Ʈ �ñ״�ó ����
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

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

	// ī�޶� ����
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


	// ������Ʈ ����
	auto m_pTutorial = new CGameObject();
	m_pTutorial->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pTutorial->SetShader(pShader);
	m_pTutorial->Rotate(0.0f, 180.0f, 0.0f); // 180�� ȸ��
	m_pTutorial->SetPosition(-0.0f, 10.0f, 30.0f);
	m_pTutorial->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "Tutorial.obj", XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)));
	objects.push_back(m_pTutorial);

	auto m_pLv1 = new CGameObject();
	m_pLv1->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pLv1->SetShader(pShader);
	m_pLv1->Rotate(0.0f, 180.0f, 0.0f); // 180�� ȸ��
	m_pLv1->SetPosition(-10.0f, 0.0f, 30.0f);
	m_pLv1->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "Lv1.obj", XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)));
	objects.push_back(m_pLv1);

	auto m_pLv2 = new CGameObject();
	m_pLv2->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pLv2->SetShader(pShader);
	m_pLv2->Rotate(0.0f, 180.0f, 0.0f); // 180�� ȸ��
	m_pLv2->SetPosition(10.0f, 0.0f, 30.0f);
	m_pLv2->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "Lv2.obj", XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)));
	objects.push_back(m_pLv2);

	auto m_pStart = new CGameObject();
	m_pStart->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pStart->SetShader(pShader);
	m_pStart->Rotate(0.0f, 180.0f, 0.0f); // 180�� ȸ��
	m_pStart->SetPosition(-10.0f, -10.0f, 30.0f);
	m_pStart->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "Start.obj", XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)));
	objects.push_back(m_pStart);

	auto m_pEnd = new CGameObject();
	m_pEnd->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pEnd->SetShader(pShader);
	m_pEnd->Rotate(0.0f, 180.0f, 0.0f); // 180�� ȸ��
	m_pEnd->SetPosition(10.0f, -10.0f, 30.0f);
	m_pEnd->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "End.obj", XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)));
	objects.push_back(m_pEnd);
}

void MenuScene::ReleaseObjects()
{
}

void MenuScene::Render(ID3D12GraphicsCommandList* pd3dCommandList)
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

	if (pPlayer) pPlayer->Render(pd3dCommandList);
}

ID3D12RootSignature* MenuScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	return CScene::CreateGraphicsRootSignature(pd3dDevice); // CScene ���� ��Ʈ �ñ״�ó ���
}

void MenuScene::AnimateObjects(float fTimeElapsed)
{
	if (pPlayer) pPlayer->Update(fTimeElapsed); // �÷��̾�(�� ī�޶�) ������Ʈ
	for (auto& obj : objects)
	{
		if (obj) obj->Animate(fTimeElapsed); // �� ������Ʈ ������Ʈ
	}
	CScene::AnimateObjects(fTimeElapsed);       // ���� ���̴� ��ü�� ������Ʈ
}

CGameObject* MenuScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	if (!pCamera) return(NULL);
	XMFLOAT4X4 xmf4x4View = pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = pCamera->GetProjectionMatrix();
	D3D12_VIEWPORT d3dViewport = pCamera->GetViewport();
	XMFLOAT3 xmf3PickPosition;
	/*ȭ�� ��ǥ���� �� (xClient, yClient)�� ȭ�� ��ǥ ��ȯ�� ����ȯ�� ���� ��ȯ�� ����ȯ�� �Ѵ�. �� ����� ī�޶�
   ��ǥ���� ���̴�. ���� ����� ī�޶󿡼� z-������ �Ÿ��� 1�̹Ƿ� z-��ǥ�� 1�� �����Ѵ�.*/
	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;
	int nIntersected = 0;
	float fHitDistance = FLT_MAX, fNearestHitDistance = FLT_MAX;
	CGameObject* pIntersectedObject = NULL, * pNearestObject = NULL;
	//���̴��� ��� ���� ��ü�鿡 ���� ���콺 ��ŷ�� �����Ͽ� ī�޶�� ���� ����� ���� ��ü�� ���Ѵ�.
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
