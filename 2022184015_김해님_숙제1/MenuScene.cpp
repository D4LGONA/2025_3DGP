#include "stdafx.h"
#include "MenuScene.h"
#include "GraphicsPipeline.h"
#include "GameFramework.h"

CCamera* CMenuScene::CreateCamera()
{
	CCamera* pCamera = new CCamera();
	pCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	pCamera->GeneratePerspectiveProjectionMatrix(1.01f, 500.0f, 60.0f);
	pCamera->SetFOVAngle(60.0f);

	pCamera->GenerateOrthographicProjectionMatrix(1.01f, 50.0f, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	return pCamera;
}

void CMenuScene::BuildObjects()
{
	CCubeMesh* t = new CCubeMesh();
	m_pPlayer = new CPlayer();
	m_pPlayer->SetPosition(0.0f, 0.0, 0.0f);
	m_pPlayer->SetMesh(nullptr);
	m_pPlayer->SetColor(RGB(0, 0, 255));
	m_pPlayer->SetCamera(CreateCamera());
	m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 0.0f, -20.0f));

	CObjMesh* pMesh = new CObjMesh("Tutorial.obj");
	m_pTutorial = new CExplosiveObject();
	m_pTutorial->SetPosition(-0.0f, 10.0f, 10.0f);
	m_pTutorial->SetMesh(pMesh);
	m_pTutorial->SetColor(RGB(0, 0, 0));
	m_pTutorial->Rotate(m_pTutorial->GetUp(), 180.0f);

	pMesh = new CObjMesh("Lv1.obj");
	m_pLv1 = new CExplosiveObject();
	m_pLv1->SetPosition(-10.0f, 0.0f, 10.0f);
	m_pLv1->SetMesh(pMesh);
	m_pLv1->SetColor(RGB(0, 0, 0));
	m_pLv1->Rotate(m_pLv1->GetUp(), 180.0f);

	pMesh = new CObjMesh("Lv2.obj");
	m_pLv2 = new CExplosiveObject();
	m_pLv2->SetPosition(10.0f, 0.0f, 10.0f);
	m_pLv2->SetMesh(pMesh);
	m_pLv2->SetColor(RGB(0, 0, 0));
	m_pLv2->Rotate(m_pLv2->GetUp(), 180.0f);

	pMesh = new CObjMesh("Start.obj");
	m_pStart = new CExplosiveObject();
	m_pStart->SetPosition(-10.0f, -10.0f, 10.0f);
	m_pStart->SetMesh(pMesh);
	m_pStart->SetColor(RGB(0, 0, 0));
	m_pStart->Rotate(m_pStart->GetUp(), 180.0f);

	pMesh = new CObjMesh("End.obj");
	m_pEnd = new CExplosiveObject();
	m_pEnd->SetPosition(10.0f, -10.0f, 10.0f);
	m_pEnd->SetMesh(pMesh);
	m_pEnd->SetColor(RGB(0, 0, 0));
	m_pEnd->Rotate(m_pEnd->GetUp(), 180.0f);
}

void CMenuScene::ReleaseObjects()
{
}

void CMenuScene::Animate(float fElapsedTime)
{
	if (curPickingObject != nullptr)
		curPickingObject->SetColor(RGB(rand() % 256, rand() % 256, rand() % 256));
	m_pTutorial->Animate(fElapsedTime);
}

void CMenuScene::Render(HDC hDCFrameBuffer)
{
	auto pCamera = m_pPlayer->GetCamera();

	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);

	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);

	m_pTutorial->Render(hDCFrameBuffer, pCamera);
	m_pLv1->Render(hDCFrameBuffer, pCamera);
	m_pLv2->Render(hDCFrameBuffer, pCamera);
	m_pStart->Render(hDCFrameBuffer, pCamera);
	m_pEnd->Render(hDCFrameBuffer, pCamera);
}

void CMenuScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (nMessageID == WM_MOUSEMOVE)
	{
		curPickingObject = PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam), m_pPlayer->m_pCamera);
		if (curPickingObject != nullptr) {
			doChangeColor = true;
		}
		else {
			if (doChangeColor == true)
			{
				m_pTutorial->SetColor(RGB(0, 0, 0));
				m_pLv1->SetColor(RGB(0, 0, 0));
				m_pLv2->SetColor(RGB(0, 0, 0));
				m_pStart->SetColor(RGB(0, 0, 0));
				m_pEnd->SetColor(RGB(0, 0, 0));
			}
			doChangeColor = false;
		}
	}
	
	if(nMessageID == WM_LBUTTONDOWN)
	{
		if (curPickingObject == m_pStart)
		{
			CGameFramework::ChangeScene = true;
			CGameFramework::idx = 2;
		}
		else if (curPickingObject == m_pEnd)
		{
			exit(0);
		}
	}
}

void CMenuScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CMenuScene::ProcessInput(POINT oldCursorPos, HWND hWnd, float m_fElapsedTime)
{
}

// todo
CGameObject* CMenuScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / (float)pCamera->m_Viewport.m_nWidth) - 1) / pCamera->m_xmf4x4PerspectiveProject._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / (float)pCamera->m_Viewport.m_nHeight) - 1) / pCamera->m_xmf4x4PerspectiveProject._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&pCamera->m_xmf4x4View);

	int nIntersected = 0;

	float fHitDistance = FLT_MAX;
	if(m_pTutorial->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fHitDistance) > 0)
	{
		return m_pTutorial;
	}
	else if (m_pLv1->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fHitDistance) > 0)
	{
		return m_pLv1;
	}
	else if (m_pLv2->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fHitDistance) > 0)
	{
		return m_pLv2;
	}
	else if (m_pStart->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fHitDistance) > 0)
	{
		return m_pStart;
	}
	else if (m_pEnd->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fHitDistance) > 0)
	{
		return m_pEnd;
	}
	else
	{
		return nullptr;
	}
}
