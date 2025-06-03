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

void TitleScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	// ī�޶� ���ų� ������Ʈ�� ���ٸ� ������ ����
	if (!pPlayer || !pPlayer->GetCamera()) return;

	// ī�޶� ���̴� ��� ������Ʈ
	pPlayer->GetCamera()->UpdateShaderVariables(pd3dCommandList);

	// �÷��̾� ���̴� ��� ������Ʈ (��: ���� ��� ��)
	pPlayer->UpdateShaderVariables(pd3dCommandList);

	// ������Ʈ ������
	for (auto& obj : objects)
	{
		if (obj) obj->Render(pd3dCommandList, pPlayer->GetCamera());
	}
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

void TitleScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// ��Ʈ �ñ״�ó ����
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	// �÷��̾� ����
	//pPlayer = new CPlayer();

	////�÷��̾ ���� ���̴� ������ �����Ѵ�. 
	//pPlayer->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	////�÷��̾��� ��ġ�� �����Ѵ�. 
	//pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	////�÷��̾�(�����) �޽��� �������� �� ����� ���̴��� �����Ѵ�.
	//CObjectsShader* pShader = new CObjectsShader();
	//pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	//pPlayer->SetShader(pShader);

	//// ī�޶� ����
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


	// Ÿ��Ʋ�� ���̴� ����
	m_pTitleShader = new CInstancingShader();
	m_pTitleShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pTitleShader->BuildObjects(pd3dDevice, pd3dCommandList);

	// ������Ʈ ����
	// ������Ʈ 0: 3D���α׷���1 �޽�


	m_pShaders.push_back(m_pTitleShader);
}

void TitleScene::ReleaseObjects()
{
}
