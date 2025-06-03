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

	//// �÷��̾� ���̴� ��� ������Ʈ (��: ���� ��� ��)
	//pPlayer->UpdateShaderVariables(pd3dCommandList);

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
	objects.push_back(new CGameObject());
	objects[0]->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	objects[0]->SetShader(pShader);
	objects[0]->SetPosition(XMFLOAT3(0.0f, 10.0f, 50.0f));
	objects[0]->Rotate(0.0f, 180.0f, 0.0f); // 180�� ȸ��
	objects[0]->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "3DGP.obj", XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)));

	objects.push_back(new CGameObject());
	objects[1]->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	objects[1]->SetShader(pShader);
	objects[1]->SetPosition(XMFLOAT3(0.0f, -10.0f, 50.0f));
	objects[1]->Rotate(0.0f, 180.0f, 0.0f); // 180�� ȸ��
	objects[1]->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "NAME.obj", XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
}

void TitleScene::ReleaseObjects()
{
}
