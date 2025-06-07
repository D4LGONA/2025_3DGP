#include "stdafx.h"
#include "Scene1.h"
#include "Player.h"

static XMFLOAT3 Lerp(const XMFLOAT3& a, const XMFLOAT3& b, float t)
{
	return XMFLOAT3(
		a.x + (b.x - a.x) * t,
		a.y + (b.y - a.y) * t,
		a.z + (b.z - a.z) * t
	);
}

Scene1::Scene1(CGameTimer* timer)
	: CScene(timer)
{
}

Scene1::~Scene1()
{
}

int Scene1::LoadLineToMesh(const char* filename, std::vector<XMFLOAT3>& vertices, std::vector<std::array<int, 2>>& edges)
{
	std::ifstream file(filename);
	std::string line;

	while (std::getline(file, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			float x, y, z;
			sscanf_s(line.c_str(), "v %f %f %f", &x, &y, &z);
			vertices.push_back(XMFLOAT3(x, y, z));
		}
		else if (line.substr(0, 2) == "l ")
		{
			int a, b;
			std::array<int, 2> arr;
			sscanf_s(line.c_str(), "l %d %d", &a, &b);
			if (a > b)
				arr = { b - 1, a - 1 };
			else
				arr = { a - 1, b - 1 };
			edges.push_back(arr);
		}
	}
	return (int)edges.size();
}

void Scene1::BuildOrderedLinePath(const std::vector<XMFLOAT3>& vertices, const std::vector<std::array<int, 2>>& edges, std::vector<XMFLOAT3>& outPath)
{
	std::unordered_map<int, std::vector<int>> adjacency;

	for (const auto& e : edges) {
		adjacency[e[0]].push_back(e[1]);
		adjacency[e[1]].push_back(e[0]);
	}

	int start = -1;
	for (const auto& a : adjacency) {
		if (a.second.size() == 1) {
			start = a.first;
			break;
		}
	}

	if (start == -1) return;

	std::unordered_set<int> visited;
	int current = start;
	int previous = -1;

	while (true) {
		outPath.push_back(vertices[current]);
		visited.insert(current);

		const auto& neighbors = adjacency[current];
		int next = -1;

		for (int n : neighbors) {
			if (n != previous && visited.find(n) == visited.end()) {
				next = n;
				break;
			}
		}

		if (next == -1) break;

		previous = current;
		current = next;
	}
}

void Scene1::CreateGraphicsPipelineState(ID3D12Device* pd3dDevice)
{
	if (!m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CScene::CreateGraphicsPipelineState(pd3dDevice); // 공통 파이프라인 사용
}

void Scene1::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 루트 시그니처 생성
	if (build == false) {
		m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
		build = true;
	}

	// 트랙 생성
	std::vector<XMFLOAT3> tmps;
	LoadLineToMesh("track_line.obj", tmps, m_vTrackEdges);
	BuildOrderedLinePath(tmps, m_vTrackEdges, m_vTrackPoints);

	if (Vector3::Length(m_vTrackPoints.front()) > Vector3::Length(m_vTrackPoints.back()))
	{
		std::reverse(m_vTrackPoints.begin(), m_vTrackPoints.end());
	}

	// 플레이어 생성
	m_pPlayer = new CPlayer();
	m_pPlayer->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	CDiffusedShader* pShader = new CDiffusedShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pPlayer->SetShader(pShader);
	m_pPlayer->SetMesh(new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 5.0f, 5.0f, 5.0f)); // 포신으로 머리.

	// 카메라 설정 -> 3인칭 카메라로 변경해줘야 함.
	CCamera* pCamera = new CCamera();
	m_pPlayer->SetFriction(200.0f);
	m_pPlayer->SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pPlayer->SetMaxVelocityXZ(125.0f);
	m_pPlayer->SetMaxVelocityY(400.0f);
	m_pPlayer->SetCamera(pCamera);
	m_pPlayer->SetCamera(m_pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, m_SceneTimer->GetTimeElapsed()));

	// 트랙 설정
	m_pTrack = new CGameObject();
	m_pTrack->SetMesh(new CObjMeshDiffused(pd3dDevice, pd3dCommandList, "track_mesh_fixed.obj", {1.0f, 0.5, 0.5f, 1.0f})); // 이 부분은 직접 구현 필요
	m_pTrack->SetShader(pShader);
	m_pTrack->SetPosition(XMFLOAT3(0.0f, -5.0f, 0.0f));
}

void Scene1::ReleaseObjects()
{
	if (m_pTrack) delete m_pTrack;
	if (m_vTrackPoints.size() > 0)
	{
		m_vTrackPoints.clear();
	}
	if (m_vTrackEdges.size() > 0)
	{
		m_vTrackEdges.clear();
	}
}

ID3D12RootSignature* Scene1::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	return CScene::CreateGraphicsRootSignature(pd3dDevice); // CScene 공통 루트 시그니처 사용
}

void Scene1::AnimateObjects(float fTimeElapsed)
{
	if (m_iCurrentPathIndex >= m_vTrackPoints.size()) {
		timer += fTimeElapsed;
		if (timer >= 2.0f) {
			change = true;
			idx = 3;
		}
		return;
	}
	timer = 0.0f;

	XMFLOAT3 curPos = m_pPlayer->GetPosition();
	XMFLOAT3 targetPos = m_vTrackPoints[m_iCurrentPathIndex];

	// 근처에 있는지 확인
	XMFLOAT3 direction = Vector3::Subtract(targetPos, curPos);
	float distance = Vector3::Length(direction);
	if (distance < 0.001f)
	{
		m_iCurrentPathIndex++;
		return;
	}

	XMFLOAT3 forwardDir = Vector3::Normalize(direction);

	// 경사도 기반 가속도 적용
	float heightDelta = targetPos.y - curPos.y;
	float accel = (heightDelta < 0) ? -heightDelta * 30.0f : -heightDelta * 0.8f;

	m_fCurrentSpeed += accel * fTimeElapsed;
	m_fCurrentSpeed = std::clamp(m_fCurrentSpeed, 2.0f, 50.0f);

	// 이동 거리
	float moveDistance = m_fCurrentSpeed * fTimeElapsed;

	// 멀리 지나치는가 ?
	if (distance <= moveDistance)
	{
		m_pPlayer->SetPosition(targetPos);
		m_iCurrentPathIndex++;
		return;
	}

	// 이동 처리
	XMFLOAT3 moveVec = Vector3::ScalarProduct(forwardDir, moveDistance);
	m_pPlayer->Move(moveVec, false);

	// 방향 보간
	XMFLOAT3 normal = ((CTrackMesh*)m_pTrack->GetMesh())->GetNormal(m_iCurrentPathIndex);
	if (Vector3::DotProduct(normal, m_pPlayer->GetUp()) < 0)
		normal = Vector3::ScalarProduct(normal, -1.0f);

	XMFLOAT3 look = forwardDir;
	XMFLOAT3 right = Vector3::Normalize(Vector3::CrossProduct(look, normal));
	XMFLOAT3 up = Vector3::CrossProduct(right, look);

	float alpha = 5.0f * fTimeElapsed;

	XMFLOAT3 prevLook = m_pPlayer->GetLook();
	XMFLOAT3 prevUp = m_pPlayer->GetUp();
	XMFLOAT3 prevRight = m_pPlayer->GetRight();

	XMFLOAT3 newLook = Vector3::Normalize(Lerp(prevLook, look, alpha));
	XMFLOAT3 newUp = Vector3::Normalize(Lerp(prevUp, up, alpha));
	XMFLOAT3 newRight = Vector3::Normalize(Vector3::CrossProduct(newUp, newLook));
	newUp = Vector3::CrossProduct(newLook, newRight);

	// Yaw
	float yaw = atan2f(newLook.x, newLook.z) - atan2f(prevLook.x, prevLook.z);
	yaw = XMConvertToDegrees(yaw);

	m_pPlayer->Rotate(0.0f, yaw, 0.0f);

	// 애니메이션
	m_pPlayer->Animate(fTimeElapsed);
	m_pPlayer->Update(fTimeElapsed);
	m_pTrack->Animate(fTimeElapsed);
}

void Scene1::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 카메라가 없거나 오브젝트가 없다면 렌더링 생략
	if (!m_pPlayer || !m_pPlayer->GetCamera()) return;

	auto pCamera = m_pPlayer->GetCamera();

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);


	m_pTrack->Render(pd3dCommandList, pCamera); // 맵 렌더링

#ifdef _WITH_PLAYER_TOP
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif

	if (m_pPlayer) {
		m_pPlayer->Render(pd3dCommandList, m_pPlayer->GetCamera());
	}
}

bool Scene1::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (nMessageID == WM_KEYUP)
	{
		switch (wParam)
		{
		case 'N':
		case 'n':
			change = true;
			idx = 3;
			break;
		case VK_ESCAPE:
			change = true;
			idx = 1;
			break;
		}
		return false;
	}
}

bool Scene1::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void Scene1::ProcessInput(const UCHAR* pKeyBuffer, float cxDelta, float cyDelta, float timeElapsed)
{
	if (m_pPlayer)
	{
		if (cxDelta || cyDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}
		m_pPlayer->Update(timeElapsed);
	}
}

void Scene1::Reset(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_iCurrentPathIndex = 0;
	m_fCurrentSpeed = 20.0f;
	timer = 0.0f;
	m_pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
}
