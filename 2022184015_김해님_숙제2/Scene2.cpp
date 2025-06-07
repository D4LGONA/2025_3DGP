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
		case 's':
		case'S':
			if (shieldTime < 3.0f && bShield == true) break;
			shieldTime = 0.0f;
			bShield = true;
			break;

		case 'a':
		case 'A':
			AutoTarget = !AutoTarget; // �ڵ� Ÿ���� ���
			break;

		case 'w':
		case 'W':
			// ��� expobj�� ��Ʈ����.
			for (auto& obj : enemies)
			{
				if (obj->bActive == false) continue; // CExplosiveObject�� �ƴ� ��� ����
				if (!obj->m_bBlowingUp)
				{
					obj->StartExplosion(); // ���� ����
				}
			}
			enemie_count = 0;
			break;
		case VK_CONTROL:
		{
			if (delay < 0.25f) break;
			auto pl = dynamic_cast<CTankPlayer*>(pPlayer);
				// �÷��̾ ������ ������Ʈ�� ������ �ƹ��͵� ����.
			pl->FireBullet(pickedObj);
			delay = 0.0f;
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
			if (pPlayer) 
				pPlayer->SetCamera(pPlayer->ChangeCamera((wParam - VK_F1 + 1), m_SceneTimer->GetTimeElapsed()));
			pPlayer->setPitch(0.0f);
			break;
		case VK_ESCAPE:
			change = true;
			idx = 1;
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
		if (AutoTarget == false) break;
		pickedObj = PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam), pPlayer->GetCamera());
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
	if (build == false)
	{
		m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
		build = true;
	}

	auto mesh = new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "head.obj", XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

	// �÷��̾� ����
	pPlayer = new CTankPlayer(pd3dDevice, pd3dCommandList);

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

	// ����
	pShield = new CGameObject();
	pShield->SetShader(pShader);
	pShield->SetMesh(new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 4.0f, 4.0f, 4.0f)); // �������� �Ӹ�.
	
	for (int i = 0; i < 5; ++i) {
		auto obj = new CObstacles(XMFLOAT3(RandF(-1.0f, 1.0f), 0.0f, 0.0f), RandF(1.0f, 5.0f));
		obj->SetShader(pShader);
		obj->SetPosition(0.0f, 0.0f, i * 15.0f + 10.0f);
		obj->SetMesh(new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 3.0f, 3.0f, 1.0f));
		obstacles.push_back(obj);
	}

	// ���޽� -> ȸ�� �������� ī�޶��� �ٶ󺸰� ����.
	// ������Ʈ ����
	for (int i = 0; i < 10; ++i) {
		auto obj = new CTankObject(); // Rotating Object�� ���� �Ŀ� ��ŷ�� �Ͼ�� Explosive Object�� ����.
		obj->setExplosionMesh(new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f));
		obj->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		obj->SetExpShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
		obj->SetShader(pShader);

		bool bOverlap = true;
		XMFLOAT3 position;

		do {
			position = XMFLOAT3(RandF(-5.0f, 5.0f), 0.0f, RandF(-0.0f, 10.0f) + 10.0f);
			bOverlap = false;

			// ���� ������ �Ÿ� Ȯ��
			for (auto& ob : enemies)
			{
				if (ob == nullptr) continue;

				XMFLOAT3 other = ob->GetPosition();
				float dx = position.x - other.x;
				float dz = position.z - other.z;
				float dist = sqrtf(dx * dx + dz * dz);
				if (dist <= 3.0f) {
					bOverlap = true;
					break;
				}
			}

			// ��ֹ����� �Ÿ� Ȯ��
			if (!bOverlap) {
				for (auto& obs : obstacles)
				{
					if (obs == nullptr) continue;

					XMFLOAT3 other = obs->GetPosition();
					float dx = position.x - other.x;
					float dz = position.z - other.z;
					float dist = sqrtf(dx * dx + dz * dz);
					if (dist <= 3.0f) {
						bOverlap = true;
						break;
					}
				}
			}
		} while (bOverlap);

		obj->SetPosition(position);
		obj->SetMesh(mesh);
		obj->SetBody(new CCubeMeshDiffused(pd3dDevice, pd3dCommandList));
		obj->Rotate(0.0f, 180.0f, 0.0f);
		enemies.push_back(obj);
	}

	map = new CGameObject();
	map->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	map->SetShader(pShader);
	map->SetPosition(0.0f, 6.0f, 0.0f);
	map->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "map.obj", XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f)));

	YouWinObject = new CGameObject();
	YouWinObject->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	YouWinObject->SetShader(pShader);
	YouWinObject->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "YouWin.obj", XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)));
	YouWinObject->Rotate(0.0f, 180.0f, 0.0f);
}

void Scene2::Reset(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	auto mesh = new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "head.obj", XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

	pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	CCamera* pCamera = new CCamera();
	pPlayer->SetFriction(200.0f);
	pPlayer->SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
	pPlayer->SetMaxVelocityXZ(125.0f);
	pPlayer->SetMaxVelocityY(400.0f);
	pPlayer->SetCamera(pCamera);
	pPlayer->SetCamera(pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, m_SceneTimer->GetTimeElapsed()));

	pBody->SetPosition(pPlayer->GetPosition().x, pPlayer->GetPosition().y - 1.0f, pPlayer->GetPosition().z);

	for (int i = 0; i < 10; ++i) {

		enemies[i]->bActive = true;
		enemies[i]->GetBody()->bActive = true;
		enemies[i]->m_bBlowingUp = false;

		bool bOverlap = true;
		XMFLOAT3 position;

		do {
			position = XMFLOAT3(RandF(-5.0f, 5.0f), 0.0f, RandF(-0.0f, 20.0f) + 10.0f);
			bOverlap = false;

			// ���� ������ �Ÿ� Ȯ��
			for (auto& ob : enemies)
			{
				if (ob == nullptr) continue;

				XMFLOAT3 other = ob->GetPosition();
				float dx = position.x - other.x;
				float dz = position.z - other.z;
				float dist = sqrtf(dx * dx + dz * dz);
				if (dist <= 3.0f) {
					bOverlap = true;
					break;
				}
			}

			// ��ֹ����� �Ÿ� Ȯ��
			if (!bOverlap) {
				for (auto& obs : obstacles)
				{
					if (obs == nullptr) continue;

					XMFLOAT3 other = obs->GetPosition();
					float dx = position.x - other.x;
					float dz = position.z - other.z;
					float dist = sqrtf(dx * dx + dz * dz);
					if (dist <= 3.0f) {
						bOverlap = true;
						break;
					}
				}
			}
		} while (bOverlap);

		enemies[i]->SetPosition(position);
		enemies[i]->Rotate(0.0f, 180.0f, 0.0f);
	}

	YouWinObject->Rotate(0.0f, 0.0f, 0.0f);

	// ���� ���� �ʱ�ȭ
	bShield = false;
	shieldTime = 0.0f;
	delay = 0.0f;
	pickedObj = nullptr;
	AutoTarget = false;
	hp = 3;
	enemie_count = 10;
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


	map->Render(pd3dCommandList, pCamera); // �� ������
	
	for (auto& obj : obstacles)
		obj->Render(pd3dCommandList, pPlayer->GetCamera());

	// ������Ʈ ������
	for (auto& obj : enemies)
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

	if (bShield == true) {
		pShield->SetPosition(pPlayer->GetPosition().x, pPlayer->GetPosition().y - 0.5f, pPlayer->GetPosition().z);
		pShield->Render(pd3dCommandList, pPlayer->GetCamera());
	}

	if (enemie_count <= 0)
	{
		YouWinObject->Render(pd3dCommandList, pPlayer->GetCamera());
	}

}

ID3D12RootSignature* Scene2::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	return CScene::CreateGraphicsRootSignature(pd3dDevice); // CScene ���� ��Ʈ �ñ״�ó ���
}

void Scene2::AnimateObjects(float fTimeElapsed)
{
	if (enemie_count <= 0)
	{
		YouWinObject->SetPosition(pPlayer->GetPosition().x, pPlayer->GetPosition().y, pPlayer->GetPosition().z + 5.0f);
	}
	if (hp <= 0)
	{
		change = true;
		idx = 1; 
		return;
	}

	delay += fTimeElapsed;
	shieldTime += fTimeElapsed;
	if (shieldTime > 3.0f && bShield == true) bShield = false;

	auto pl = dynamic_cast<CTankPlayer*>(pPlayer);
	if (pl) pl->Animate(fTimeElapsed); // �÷��̾�(�� ī�޶�) ������Ʈ
	if (pl) pl->Update(fTimeElapsed); // �÷��̾�(�� ī�޶�) ������Ʈ
	for (auto& obj : enemies)
	{
		if (obj) obj->Animate(fTimeElapsed); // �� ������Ʈ ������Ʈ
	}
	for (auto& obj : obstacles)
		obj->Animate(fTimeElapsed);
	CScene::AnimateObjects(fTimeElapsed);       // ���� ���̴� ��ü�� ������Ʈ

	
	// �浹üũ
	CheckEnemyByBulletCollisions();

	CheckObstacleByBulletCollisions();

	CheckEnemyByWallCollisions();

	CheckEnemyByEnemyCollisions();

	CheckObstacleByWallCollisions();

	CheckEnemyByObstacleCollision();

	CheckEnemyByPlayerCollision();
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

	for (auto& obj : enemies)
	{
		if (obj->bActive == false) continue;
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

		float speed = 500.0f * timeElapsed;

		XMFLOAT3 curPos = pPlayer->GetPosition();
		XMFLOAT3 look = pPlayer->GetLook();
		XMFLOAT3 right = pPlayer->GetRight();
		XMVECTOR moveVec = XMVectorZero();

		if (dwDirection & DIR_FORWARD) moveVec += XMLoadFloat3(&look);
		if (dwDirection & DIR_BACKWARD) moveVec -= XMLoadFloat3(&look);
		if (dwDirection & DIR_LEFT) moveVec -= XMLoadFloat3(&right);
		if (dwDirection & DIR_RIGHT) moveVec += XMLoadFloat3(&right);

		moveVec = XMVector3Normalize(moveVec) * speed;
		XMFLOAT3 nextPos;
		XMStoreFloat3(&nextPos, XMLoadFloat3(&curPos) + moveVec);

		BoundingOrientedBox nextBox = pBody->GetBoundingBox();
		nextBox.Center = nextPos;

		bool bCollision = false;
		for (auto& obs : obstacles)
		{
			if (obs->GetBoundingBox().Intersects(nextBox))
			{
				bCollision = true;
				break;
			}
		}

		if (!bCollision && dwDirection)
			pPlayer->Move(dwDirection, speed, true);

		if (cxDelta || cyDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}

		pPlayer->Update(timeElapsed);
		pBody->SetPosition(pPlayer->GetPosition().x, pPlayer->GetPosition().y - 1.0f, pPlayer->GetPosition().z);
		ClampPlayerBodyPosition();
	}
}


void Scene2::CheckEnemyByBulletCollisions()
{
	CBulletObject** ppBullets = ((CTankPlayer*)pPlayer)->m_ppBullets;
	for (int i = 0; i < enemies.size(); i++)
	{
		if (enemies[i]->bActive == false) continue;
		for (int j = 0; j < BULLETS; j++)
		{
			if (ppBullets[j]->bActive && enemies[i]->GetBoundingBox().Intersects(ppBullets[j]->GetBoundingBox()))
			{
				if (enemies[i]->m_bBlowingUp == true) continue;
				enemies[i]->StartExplosion();
				ppBullets[j]->Reset();
				enemie_count--;
			}
		}
	}
}

void Scene2::CheckObstacleByBulletCollisions()
{
	CBulletObject** ppBullets = ((CTankPlayer*)pPlayer)->m_ppBullets;
	for (int i = 0; i < obstacles.size(); i++)
	{
		for (int j = 0; j < BULLETS; j++)
		{
			if (ppBullets[j]->bActive && obstacles[i]->GetBoundingBox().Intersects(ppBullets[j]->GetBoundingBox()))
			{
				ppBullets[j]->Reset();
			}
		}
	}
}

void Scene2::CheckEnemyByWallCollisions()
{
	for (int i = 0; i < enemies.size(); i++) // ��ũ��
	{
		if (enemies[i]->bActive == false) continue;

		XMFLOAT3 bodyPos = enemies[i]->GetPosition();

		if (bodyPos.x <= -7.0f || bodyPos.x >= 7.0f) {
			enemies[i]->m_xmf3MoveDirection.x *= -1.0f; // X�� ����
		}
	}
}

void Scene2::CheckObstacleByWallCollisions()
{
	for (int i = 0; i < obstacles.size(); i++) // ��ũ��
	{
		if (obstacles[i]->GetPosition().x <= -7.0f || obstacles[i]->GetPosition().x >= 7.0f) {
			obstacles[i]->ChangeDir(); // X�� ����
		}
	}
}


void Scene2::CheckEnemyByEnemyCollisions()
{
	for (int i = 0; i < enemies.size(); i++)
	{
		for (int j = i + 1; j < enemies.size(); ++j)
		{
			// �浹 ���� (OBB ����)
			if (enemies[i]->GetBody()->GetBoundingBox().Intersects(enemies[j]->GetBody()->GetBoundingBox()))
			{
				std::swap(enemies[i]->m_xmf3MoveDirection, enemies[j]->m_xmf3MoveDirection);
			}
		}
	}
}

void Scene2::ClampPlayerBodyPosition()
{
	if (!pBody || !pPlayer) return;

	XMFLOAT3 bodyPos = pBody->GetPosition();

	// XZ ��� ����
	bodyPos.x = std::clamp(bodyPos.x, -7.0f, 7.0f);

	// ��ġ ����
	pBody->SetPosition(bodyPos);

	// pPlayer�� ���� �̵� (�Ӹ� ��ġ = ��ü ��ġ + 1.0f)
	pPlayer->SetPosition(XMFLOAT3(bodyPos.x, bodyPos.y + 1.0f, bodyPos.z));

	map->SetPosition(0.0f, 6.0f, pPlayer->GetPosition().z);
}

void Scene2::CheckEnemyByObstacleCollision()
{
	for (int i = 0; i < enemies.size(); ++i)
	{
		if (!enemies[i] || enemies[i]->bActive == false) continue;
		for (int j = 0; j < obstacles.size(); ++j)
		{
			if (enemies[i]->GetBody()->GetBoundingBox().Intersects(obstacles[j]->GetBoundingBox()))
			{
				XMFLOAT3 enemyPos = enemies[i]->GetPosition();
				XMFLOAT3 obstaclePos = obstacles[j]->GetPosition();

				float dx = fabs(enemyPos.x - obstaclePos.x);
				float dz = fabs(enemyPos.z - obstaclePos.z);

				if (dx > dz)
				{
					enemies[i]->m_xmf3MoveDirection.x *= -1.0f;
				}
				else
				{
					enemies[i]->m_xmf3MoveDirection.z *= -1.0f;
				}
			}
		}
	}
}

void Scene2::CheckEnemyByPlayerCollision()
{
	if (bShield == true) return;
	for (int i = 0; i < enemies.size(); ++i)
	{
		if (!enemies[i] || enemies[i]->bActive == false) continue;

		if (enemies[i]->m_bBlowingUp == false &&
			enemies[i]->GetBoundingBox().Intersects(pBody->GetBoundingBox())) // ��ü ���� �浹
		{
			enemies[i]->StartExplosion();
			hp--;
			enemie_count--;
		}
	}
}