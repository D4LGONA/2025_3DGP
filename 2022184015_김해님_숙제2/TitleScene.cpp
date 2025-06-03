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
		case 'A': // 'A' Ű ������ ��
		{
			auto a = reinterpret_cast<CExplosiveObject*>(objects.back());
			if (!a->m_bBlowingUp)
			{
				a->StartExplosion(); // ���� ����
			}
			return true; // �޽��� ó����
		}
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
			if (PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam), m_pPlayer->m_pCamera)) {
				CExplosiveObject* pExplosiveObject = (CExplosiveObject*)m_pName;
				if (pExplosiveObject->m_bBlowingUp == false) {
					pExplosiveObject->m_bBlowingUp = true;
					bChange = true;
				}
			}
		}
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		//���콺 ĸ�ĸ� �����Ѵ�. 
		::ReleaseCapture();
		break;
	}
	return false;
}

void TitleScene::CreateGraphicsPipelineState(ID3D12Device* pd3dDevice)
{
	if (!m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CScene::CreateGraphicsPipelineState(pd3dDevice); // ���� ���������� ���
}

void TitleScene::Render(ID3D12GraphicsCommandList* pd3dCommandList)
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


	//3��Ī ī�޶��� �� �÷��̾ �׻� ���̵��� �������Ѵ�. 
#ifdef _WITH_PLAYER_TOP
	//���� Ÿ���� �״�� �ΰ� ���� ���۸� 1.0���� ����� �÷��̾ �������ϸ� �÷��̾�� ������ �׷��� ���̴�. 
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif

	//3��Ī ī�޶��� �� �÷��̾ �������Ѵ�. 
	if (pPlayer) pPlayer->Render(pd3dCommandList);
}

ID3D12RootSignature* TitleScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	return CScene::CreateGraphicsRootSignature(pd3dDevice); // CScene ���� ��Ʈ �ñ״�ó ���
}

void TitleScene::AnimateObjects(float fTimeElapsed)
{
	if (pPlayer) pPlayer->Update(fTimeElapsed); // �÷��̾�(�� ī�޶�) ������Ʈ
	for (auto& obj : objects)
	{
		if (obj) obj->Animate(fTimeElapsed); // �� ������Ʈ ������Ʈ
	}
	CScene::AnimateObjects(fTimeElapsed);       // ���� ���̴� ��ü�� ������Ʈ
}

// ���̴� -> ��� ������Ʈ�� ���� DiffusedShader ���
void TitleScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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
	// ������Ʈ 0: 3D���α׷���1 �޽�
	auto nameobj = new CRotatingObject();
	nameobj->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f)); // Y���� ȸ�������� ����
	nameobj->SetRotationSpeed((((rand() / (float)RAND_MAX) * 10.0f) - 5.0f) * 10.0f);
	nameobj->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	nameobj->SetShader(pShader);
	nameobj->SetPosition(XMFLOAT3(0.0f, 10.0f, 50.0f));
	nameobj->Rotate(0.0f, 180.0f, 0.0f); // 180�� ȸ��
	nameobj->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "3DGP.obj", XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)));
	objects.push_back(nameobj);


	auto obj = new CExplosiveObject(); // Rotating Object�� ���� �Ŀ� ��ŷ�� �Ͼ�� Explosive Object�� ����.
	obj->setExplosionMesh(new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f));
	obj->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	obj->SetExpShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	obj->SetShader(pShader);
	obj->SetPosition(XMFLOAT3(0.0f, -10.0f, 50.0f));
	obj->Rotate(0.0f, 180.0f, 0.0f); // 180�� ȸ��
	obj->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "NAME.obj", XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	objects.push_back(obj);
}

void TitleScene::ReleaseObjects()
{
}

bool TitleScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / (float)pCamera->m_Viewport.m_nWidth) - 1) / pCamera->m_xmf4x4PerspectiveProject._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / (float)pCamera->m_Viewport.m_nHeight) - 1) / pCamera->m_xmf4x4PerspectiveProject._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&pCamera->m_xmf4x4View);

	int nIntersected = 0;

	float fHitDistance = FLT_MAX;
	nIntersected = m_pName->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fHitDistance);
	return nIntersected;
}