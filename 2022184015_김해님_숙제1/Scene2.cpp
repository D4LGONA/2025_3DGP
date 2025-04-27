#include "stdafx.h"
#include "Scene2.h"
#include "GraphicsPipeline.h"
#include "GameObject.h"

CScene_2::CScene_2()
{
}

CScene_2::~CScene_2()
{
}

CCamera* CScene_2::CreateCamera()
{
	CCamera* pCamera = new CCamera();
	pCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	pCamera->GeneratePerspectiveProjectionMatrix(1.01f, 500.0f, 60.0f);
	pCamera->SetFOVAngle(60.0f);

	pCamera->GenerateOrthographicProjectionMatrix(1.01f, 50.0f, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	return pCamera;
}

void CScene_2::BuildObstacles()
{
	CCubeMesh* pCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);

	m_nObjects = 5;
	m_ppObjects = new CGameObject * [m_nObjects];

	m_ppObjects[0] = new CGameObject();
	m_ppObjects[0]->SetMesh(pCubeMesh);
	m_ppObjects[0]->SetColor(RGB(255, 0, 0));
	m_ppObjects[0]->SetPosition(-13.5f, 0.0f, -14.0f);
	m_ppObjects[0]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
	m_ppObjects[0]->SetRotationSpeed(90.0f);
	m_ppObjects[0]->SetMovingDirection(XMFLOAT3(1.0f, 0.0f, 0.0f));
	m_ppObjects[0]->SetMovingSpeed(10.5f);

	m_ppObjects[1] = new CGameObject();
	m_ppObjects[1]->SetMesh(pCubeMesh);
	m_ppObjects[1]->SetColor(RGB(0, 0, 255));
	m_ppObjects[1]->SetPosition(+13.5f, 0.0f, -14.0f);
	m_ppObjects[1]->SetRotationAxis(XMFLOAT3(1.0f, 1.0f, 0.0f));
	m_ppObjects[1]->SetRotationSpeed(180.0f);
	m_ppObjects[1]->SetMovingDirection(XMFLOAT3(-1.0f, 0.0f, 0.0f));
	m_ppObjects[1]->SetMovingSpeed(8.8f);

	m_ppObjects[2] = new CGameObject();
	m_ppObjects[2]->SetMesh(pCubeMesh);
	m_ppObjects[2]->SetColor(RGB(0, 255, 0));
	m_ppObjects[2]->SetPosition(0.0f, +5.0f, 20.0f);
	m_ppObjects[2]->SetRotationAxis(XMFLOAT3(1.0f, 1.0f, 0.0f));
	m_ppObjects[2]->SetRotationSpeed(30.15f);
	m_ppObjects[2]->SetMovingDirection(XMFLOAT3(1.0f, -1.0f, 0.0f));
	m_ppObjects[2]->SetMovingSpeed(5.2f);

	m_ppObjects[3] = new CGameObject();
	m_ppObjects[3]->SetMesh(pCubeMesh);
	m_ppObjects[3]->SetColor(RGB(0, 255, 255));
	m_ppObjects[3]->SetPosition(0.0f, 0.0f, 0.0f);
	m_ppObjects[3]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
	m_ppObjects[3]->SetRotationSpeed(40.6f);
	m_ppObjects[3]->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_ppObjects[3]->SetMovingSpeed(20.4f);

	m_ppObjects[4] = new CGameObject();
	m_ppObjects[4]->SetMesh(pCubeMesh);
	m_ppObjects[4]->SetColor(RGB(128, 0, 255));
	m_ppObjects[4]->SetPosition(10.0f, 0.0f, 0.0f);
	m_ppObjects[4]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_ppObjects[4]->SetRotationSpeed(50.06f);
	m_ppObjects[4]->SetMovingDirection(XMFLOAT3(0.0f, 1.0f, 1.0f));
	m_ppObjects[4]->SetMovingSpeed(6.4f);
}

void CScene_2::BuildEnemies()
{
	CTankMesh* pTankMesh = new CTankMesh();
	CCubeMesh* pTankBody = new CCubeMesh(10.0f, 5.0f, 10.0f);

	m_nEnemies = 10;
	m_remainCount = m_nEnemies;
	m_pEnemyTanks = new CEnemyTank * [m_nEnemies];

	for (int i = 0; i < m_nEnemies; i++)
	{
		m_pEnemyTanks[i] = new CEnemyTank();

		// 메쉬, 색깔
		m_pEnemyTanks[i]->SetMesh(pTankMesh);
		m_pEnemyTanks[i]->SetColor(RGB(255, 0, 0));

		// 적당한 초기 위치 (랜덤 배치 예시)
		float x = RandF(-50.0f, 50.0f);
		float z = RandF(50.0f, 200.0f);
		m_pEnemyTanks[i]->SetPosition(x, -20.0f + 5.0f, z);
		m_pEnemyTanks[i]->Rotate(m_pEnemyTanks[i]->GetRight(), 90.0f);
		m_pEnemyTanks[i]->Rotate(m_pEnemyTanks[i]->GetUp(), 180.0f);

		// 랜덤 이동 방향
		m_pEnemyTanks[i]->SetMovingDirection(XMFLOAT3(RandF(-1.0f, 1.0f), 0.0f, RandF(-1.0f, 1.0f)));
		m_pEnemyTanks[i]->SetMovingSpeed(5.0f);  // 속도
	}
}

void CScene_2::BuildObjects()
{
	CExplosiveObject::PrepareExplosion();
	BuildEnemies();
	BuildObstacles();

	float fHalfWidth = 50.0f, fHalfHeight = 20.0f, fHalfDepth = 200.0f;
	CWallMesh* pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 30);

	// 맵 초기화
	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pWallsObject->SetMesh(pWallCubeMesh);
	m_pWallsObject->SetColor(RGB(0, 0, 0));
	m_pWallsObject->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fHalfDepth);
	m_pWallsObject->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fHalfDepth);
	m_pWallsObject->m_xmOOBBPlayerMoveCheck = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth * 0.05f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	// 플레이어 초기화
	CTankMesh* pTankMesh = new CTankMesh();

	m_pPlayer = new CTankPlayer();
	m_pPlayer->SetPosition(0.0f, -20.0f + 5.0f, 0.0f);
	m_pPlayer->SetMesh(pTankMesh);
	m_pPlayer->SetColor(RGB(0, 0, 255));
	m_pPlayer->SetCamera(CreateCamera());
	m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 20.0f, -20.0f));

	CCubeMesh* pTankBody = new CCubeMesh(10.0f, 5.0f, 10.0f);
	m_PlayerBody = new CGameObject();
	m_PlayerBody->SetPosition(0.0f, -20.0f + 5.0f, 0.0f);
	m_PlayerBody->SetMesh(pTankBody);
	m_PlayerBody->SetColor(RGB(0, 0, 255));

	CWinMesh* pWinMesh = new CWinMesh();
	m_YouWinObject = new CGameObject();
	m_YouWinObject->SetMesh(pWinMesh);
	m_YouWinObject->SetPosition(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y + 10.0f, m_pPlayer->GetPosition().z + 10.0f);
	m_YouWinObject->Rotate(m_YouWinObject->GetLook(), 180.0f);
	m_YouWinObject->Rotate(m_YouWinObject->GetRight(), -90.0f);

#ifdef _WITH_DRAW_AXIS
	m_pWorldAxis = new CGameObject();
	CAxisMesh* pAxisMesh = new CAxisMesh(0.5f, 0.5f, 0.5f);
	m_pWorldAxis->SetMesh(pAxisMesh);
#endif
}

void CScene_2::ReleaseObjects()
{
	if (CExplosiveObject::m_pExplosionMesh) CExplosiveObject::m_pExplosionMesh->Release();

	for (int i = 0; i < m_nObjects; i++) if (m_ppObjects[i]) delete m_ppObjects[i];
	if (m_ppObjects) delete[] m_ppObjects;

	if (m_pWallsObject) delete m_pWallsObject;

#ifdef _WITH_DRAW_AXIS
	if (m_pWorldAxis) delete m_pWorldAxis;
#endif
}

void CScene_2::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
		if (nMessageID == WM_RBUTTONDOWN && m_bAutoAttack) {
			m_pLockedObject = PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam), m_pPlayer->m_pCamera);
		}
		break;
	default:
		break;
	}
}

void CScene_2::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			break;
		case 'A':
		case 'a':
			m_bAutoAttack = !m_bAutoAttack;
			break;
		case 'S':
		case 's':
			m_bShieldOn = !m_bShieldOn;
			break;
		case 'w':
		case 'W':
			// 여기 승리 조건이 들어가야 함
			for (int i = 0; i < m_nEnemies; ++i) {
				if (m_pEnemyTanks[i]->m_bActive == true) {
					CExplosiveObject* pExplosiveObject = (CExplosiveObject*)m_pEnemyTanks[i];
					if (pExplosiveObject->m_bBlowingUp == true) continue;
					pExplosiveObject->m_bBlowingUp = true;
					m_remainCount--;
				}
			}
			break;

		case VK_CONTROL:
			
			((CTankPlayer*)m_pPlayer)->FireBullet(m_pLockedObject);
			m_pLockedObject = NULL;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

CGameObject* CScene_2::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / (float)pCamera->m_Viewport.m_nWidth) - 1) / pCamera->m_xmf4x4PerspectiveProject._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / (float)pCamera->m_Viewport.m_nHeight) - 1) / pCamera->m_xmf4x4PerspectiveProject._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&pCamera->m_xmf4x4View);

	int nIntersected = 0;
	float fNearestHitDistance = FLT_MAX;
	CGameObject* pNearestObject = NULL;
	for (int i = 0; i < m_nEnemies; i++)
	{
		float fHitDistance = FLT_MAX;
		nIntersected = m_pEnemyTanks[i]->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fHitDistance);
		if ((nIntersected > 0) && (fHitDistance < fNearestHitDistance))
		{
			fNearestHitDistance = fHitDistance;
			pNearestObject = m_pEnemyTanks[i];
		}
	}
	return(pNearestObject);
}

void CScene_2::ProcessInput(POINT oldCursorPos, HWND hWnd, float m_fElapsedTime)
{
	static UCHAR pKeyBuffer[256];
	if (GetKeyboardState(pKeyBuffer))
	{
		DWORD dwDirection = 0;
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;

		if (dwDirection && m_pPlayer) {
			m_pPlayer->Move(dwDirection, 0.15f);
			if (m_PlayerBody) m_PlayerBody->SetPosition(m_pPlayer->GetPosition());
		}
	}

	if (GetCapture() == hWnd)
	{
		SetCursor(NULL);
		POINT ptCursorPos;
		GetCursorPos(&ptCursorPos);
		float cxMouseDelta = (float)(ptCursorPos.x - oldCursorPos.x) / 3.0f;
		float cyMouseDelta = (float)(ptCursorPos.y - oldCursorPos.y) / 3.0f;
		SetCursorPos(oldCursorPos.x, oldCursorPos.y);
		if (cxMouseDelta || cyMouseDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyMouseDelta, 0.0f, -cxMouseDelta);
			else
				m_pPlayer->Rotate(cyMouseDelta, cxMouseDelta, 0.0f);
		}
	}

	if (m_pPlayer)
		m_pPlayer->Update(m_fElapsedTime);  
}

void CScene_2::CheckObjectByObjectCollisions()
{
	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->m_pObjectCollided = NULL;
	for (int i = 0; i < m_nObjects; i++)
	{
		for (int j = (i + 1); j < m_nObjects; j++)
		{
			if (m_ppObjects[i]->m_xmOOBB.Intersects(m_ppObjects[j]->m_xmOOBB))
			{
				m_ppObjects[i]->m_pObjectCollided = m_ppObjects[j];
				m_ppObjects[j]->m_pObjectCollided = m_ppObjects[i];
			}
		}
	}
	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i]->m_pObjectCollided)
		{
			XMFLOAT3 xmf3MovingDirection = m_ppObjects[i]->m_xmf3MovingDirection;
			float fMovingSpeed = m_ppObjects[i]->m_fMovingSpeed;
			m_ppObjects[i]->m_xmf3MovingDirection = m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection;
			m_ppObjects[i]->m_fMovingSpeed = m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed;
			m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection = xmf3MovingDirection;
			m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed = fMovingSpeed;
			m_ppObjects[i]->m_pObjectCollided->m_pObjectCollided = NULL;
			m_ppObjects[i]->m_pObjectCollided = NULL;
		}
	}
}

void CScene_2::CheckEnemyTankCollisions()
{
	// [1] Enemy Tank vs Enemy Tank
	for (int i = 0; i < m_nEnemies; i++)
	{
		if (m_pEnemyTanks[i]->m_bActive == false) continue;
		for (int j = i + 1; j < m_nEnemies; j++)
		{
			if (m_pEnemyTanks[i]->m_xmOOBB.Intersects(m_pEnemyTanks[j]->m_xmOOBB))
			{
				std::swap(m_pEnemyTanks[i]->m_xmf3MovingDirection, m_pEnemyTanks[j]->m_xmf3MovingDirection);
				std::swap(m_pEnemyTanks[i]->m_fMovingSpeed, m_pEnemyTanks[j]->m_fMovingSpeed);
			}
		}
	}

	// [2] Enemy Tank vs Obstacles (벽 or 장애물)
	for (int i = 0; i < m_nEnemies; i++)
	{
		if (m_pEnemyTanks[i]->m_bActive == false) continue;
		ContainmentType containType = m_pWallsObject->m_xmOOBB.Contains(m_pEnemyTanks[i]->m_xmOOBB);
		if (containType == DISJOINT || containType == INTERSECTS)
		{
			for (int j = 0; j < 6; j++)
			{
				PlaneIntersectionType intersectType = m_pEnemyTanks[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
				if (intersectType != FRONT)
				{
					XMVECTOR normal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[j].x, m_pWallsObject->m_pxmf4WallPlanes[j].y, m_pWallsObject->m_pxmf4WallPlanes[j].z, 0.0f);
					XMVECTOR reflect = XMVector3Reflect(XMLoadFloat3(&m_pEnemyTanks[i]->m_xmf3MovingDirection), normal);
					XMStoreFloat3(&m_pEnemyTanks[i]->m_xmf3MovingDirection, reflect);
					break;
				}
			}
		}
	}
}

void CScene_2::CheckObjectByWallCollisions()
{
	for (int i = 0; i < m_nEnemies; i++)
	{
		if (m_pEnemyTanks[i]->m_bActive == false) continue;
		ContainmentType containType = m_pWallsObject->m_xmOOBB.Contains(m_pEnemyTanks[i]->m_xmOOBB);
		switch (containType)
		{
		case DISJOINT:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 6; j++)
			{
				PlaneIntersectionType intersectType = m_pEnemyTanks[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
				if (intersectType == BACK)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_pEnemyTanks[i]->m_xmf3MovingDirection), xmvNormal);
				XMStoreFloat3(&m_pEnemyTanks[i]->m_xmf3MovingDirection, xmvReflect);
			}
			break;
		}
		case INTERSECTS:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 6; j++)
			{
				PlaneIntersectionType intersectType = m_pEnemyTanks[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
				if (intersectType == INTERSECTING)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_pEnemyTanks[i]->m_xmf3MovingDirection), xmvNormal);
				XMStoreFloat3(&m_pEnemyTanks[i]->m_xmf3MovingDirection, xmvReflect);
			}
			break;
		}
		case CONTAINS:
			break;
		}
	}


	for (int i = 0; i < m_nObjects; i++)
	{
		ContainmentType containType = m_pWallsObject->m_xmOOBB.Contains(m_ppObjects[i]->m_xmOOBB);
		switch (containType)
		{
		case DISJOINT:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 6; j++)
			{
				PlaneIntersectionType intersectType = m_ppObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
				if (intersectType == BACK)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjects[i]->m_xmf3MovingDirection), xmvNormal);
				XMStoreFloat3(&m_ppObjects[i]->m_xmf3MovingDirection, xmvReflect);
			}
			break;
		}
		case INTERSECTS:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 6; j++)
			{
				PlaneIntersectionType intersectType = m_ppObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
				if (intersectType == INTERSECTING)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjects[i]->m_xmf3MovingDirection), xmvNormal);
				XMStoreFloat3(&m_ppObjects[i]->m_xmf3MovingDirection, xmvReflect);
			}
			break;
		}
		case CONTAINS:
			break;
		}
	}
}

void CScene_2::CheckPlayerByWallCollision()
{
	BoundingOrientedBox xmOOBBPlayerMoveCheck;
	m_pWallsObject->m_xmOOBBPlayerMoveCheck.Transform(xmOOBBPlayerMoveCheck, XMLoadFloat4x4(&m_pWallsObject->m_xmf4x4World));
	XMStoreFloat4(&xmOOBBPlayerMoveCheck.Orientation, XMQuaternionNormalize(XMLoadFloat4(&xmOOBBPlayerMoveCheck.Orientation)));

	if (!xmOOBBPlayerMoveCheck.Intersects(m_pPlayer->m_xmOOBB)) m_pWallsObject->SetPosition(m_pPlayer->m_xmf3Position.x, m_pPlayer->m_xmf3Position.y + 20.0f - 5.0f, m_pPlayer->m_xmf3Position.z);
}

void CScene_2::CheckObjectByBulletCollisions()
{
	CBulletObject** ppBullets = ((CTankPlayer*)m_pPlayer)->m_ppBullets;
	for (int i = 0; i < m_nObjects; i++)
	{
		for (int j = 0; j < BULLETS; j++)
		{
			if (ppBullets[j]->m_bActive && m_ppObjects[i]->m_xmOOBB.Intersects(ppBullets[j]->m_xmOOBB))
			{
				ppBullets[j]->Reset();
			}
		}
	}
}

void CScene_2::CheckEnemyByBulletCollisions()
{
	CBulletObject** ppBullets = ((CTankPlayer*)m_pPlayer)->m_ppBullets;
	for (int i = 0; i < m_nEnemies; i++)
	{
		if (m_pEnemyTanks[i]->m_bActive == false) continue;
		for (int j = 0; j < BULLETS; j++)
		{
			if (ppBullets[j]->m_bActive &&  m_pEnemyTanks[i]->m_xmOOBB.Intersects(ppBullets[j]->m_xmOOBB))
			{
				CExplosiveObject* pExplosiveObject = (CExplosiveObject*)m_pEnemyTanks[i];
				if (pExplosiveObject->m_bBlowingUp == true) continue;
				pExplosiveObject->m_bBlowingUp = true;
				ppBullets[j]->Reset();
				m_remainCount--;
			}
		}
	}
}

void CScene_2::Animate(float fElapsedTime)
{
	
	if (m_pPlayer) m_pPlayer->Animate(fElapsedTime);
	m_PlayerBody->Animate(fElapsedTime);
	
	m_pWallsObject->Animate(fElapsedTime);

	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->Animate(fElapsedTime);
	for (int i = 0; i < m_nEnemies; i++) m_pEnemyTanks[i]->Update(fElapsedTime);

	CheckPlayerByWallCollision();

	CheckObjectByWallCollisions();

	CheckEnemyTankCollisions();

	CheckObjectByBulletCollisions();

	CheckEnemyByBulletCollisions();
}

void CScene_2::Render(HDC hDCFrameBuffer)
{
	auto pCamera = m_pPlayer->GetCamera();
	if (m_remainCount == 0) {
		m_YouWinObject->SetPosition(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y + 10.0f, m_pPlayer->GetPosition().z + 10.0f);
		m_YouWinObject->Render(hDCFrameBuffer, pCamera);
	}

	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);

	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);
	m_pWallsObject->Render(hDCFrameBuffer, pCamera);
	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->Render(hDCFrameBuffer, pCamera);

	for (int i = 0; i < m_nEnemies; ++i) m_pEnemyTanks[i]->Render(hDCFrameBuffer, pCamera);
	
	if (m_pPlayer) m_pPlayer->Render(hDCFrameBuffer, pCamera);
	m_PlayerBody->Render(hDCFrameBuffer, pCamera);

	

//UI
#ifdef _WITH_DRAW_AXIS
	CGraphicsPipeline::SetViewOrthographicProjectTransform(&pCamera->m_xmf4x4ViewOrthographicProject);
	m_pWorldAxis->SetRotationTransform(&m_pPlayer->m_xmf4x4World);
	m_pWorldAxis->Render(hDCFrameBuffer, pCamera);
#endif
}
