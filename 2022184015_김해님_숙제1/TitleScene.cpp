#include "stdafx.h"
#include "TitleScene.h"
#include "GameObject.h"
#include "GraphicsPipeline.h"
#include "Player.h"
#include "GameFramework.h"

CCamera* CTitleScene::CreateCamera()
{
	CCamera* pCamera = new CCamera();
	pCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	pCamera->GeneratePerspectiveProjectionMatrix(1.01f, 500.0f, 60.0f);
	pCamera->SetFOVAngle(60.0f);

	pCamera->GenerateOrthographicProjectionMatrix(1.01f, 50.0f, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	return pCamera;
}

void CTitleScene::BuildObjects()
{
	CExplosiveObject::PrepareExplosion();

	m_pPlayer = new CPlayer();
	m_pPlayer->SetPosition(0.0f, 0.0, 0.0f);
	m_pPlayer->SetMesh(nullptr);
	m_pPlayer->SetColor(RGB(0, 0, 255));
	m_pPlayer->SetCamera(CreateCamera());
	m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 0.0f, -20.0f));

	// 이름
	CObjMesh* pName = new CObjMesh("NAME.obj");
	m_pName = new CExplosiveObject();
	m_pName->SetPosition(0.0f, -5.0f, 10.0f);
	m_pName->SetMesh(pName);
	m_pName->SetColor(RGB(0, 0, 255));
	m_pName->Rotate(m_pName->GetUp(), 180.0f);
	m_pName->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_pName->SetRotationSpeed(RandF(-30.0f, 30.0f));

	// 타이틀
	CObjMesh* p3DMesh = new CObjMesh("3DGP.obj");
	m_pTitle = new CGameObject();
	m_pTitle->SetMesh(p3DMesh);
	m_pTitle->SetPosition(0.0f, 5.0f, 10.0f);
	m_pTitle->SetColor(RGB(0, 0, 255));
	m_pTitle->Rotate(m_pTitle->GetUp(), 180.0f);
	m_pTitle->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_pTitle->SetRotationSpeed(RandF(-30.0f, 30.0f));
}

void CTitleScene::ReleaseObjects()
{
	delete m_pTitle;
	delete m_pName;
	delete m_pPlayer;
}

void CTitleScene::Animate(float fElapsedTime)
{
	if (bChange == true && ((CExplosiveObject*)m_pName)->m_bBlowingUp == false) {
		CGameFramework::ChangeScene = true;
		CGameFramework::idx = 1;
	}
	m_pName->Animate(fElapsedTime);
	m_pTitle->Animate(fElapsedTime);
	m_pPlayer->Animate(fElapsedTime);
}

void CTitleScene::Render(HDC hDCFrameBuffer)
{
	auto pCamera = m_pPlayer->GetCamera();

	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);

	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);

	m_pPlayer->Render(hDCFrameBuffer, pCamera);

	m_pName->Render(hDCFrameBuffer, pCamera);
	m_pTitle->Render(hDCFrameBuffer, pCamera);
}

void CTitleScene::ResetObjects()
{
	CExplosiveObject* pExplosiveObject = (CExplosiveObject*)m_pName;
	pExplosiveObject->m_bBlowingUp = false;
	bChange = false;
}

void CTitleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
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
		break;
	default:
		break;
	}
}

void CTitleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CTitleScene::ProcessInput(POINT oldCursorPos, HWND hWnd, float m_fElapsedTime)
{
}

bool CTitleScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
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