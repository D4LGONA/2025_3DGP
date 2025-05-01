#include "stdafx.h"
#include "Scene1.h"
#include "GraphicsPipeline.h"
#include "GameFramework.h"
#include "GameObject.h"

static XMFLOAT3 Lerp(const XMFLOAT3& a, const XMFLOAT3& b, float t)
{
	return XMFLOAT3(
		a.x + (b.x - a.x) * t,
		a.y + (b.y - a.y) * t,
		a.z + (b.z - a.z) * t
	);
}

XMVECTOR XMQuaternionRotationVectorToVector(XMVECTOR from, XMVECTOR to)
{
	XMVECTOR v0 = XMVector3Normalize(from);
	XMVECTOR v1 = XMVector3Normalize(to);
	float dot = XMVectorGetX(XMVector3Dot(v0, v1));

	if (dot >= 1.0f - 1e-6f)
		return XMQuaternionIdentity(); // 같은 방향
	if (dot <= -1.0f + 1e-6f)
	{
		// 반대 방향일 경우 → 임의의 직교 벡터로 축 설정
		XMVECTOR orthogonal = XMVector3Cross(v0, XMVectorSet(1, 0, 0, 0));
		if (XMVector3Equal(XMVector3LengthSq(orthogonal), XMVectorZero()))
			orthogonal = XMVector3Cross(v0, XMVectorSet(0, 1, 0, 0));
		orthogonal = XMVector3Normalize(orthogonal);
		return XMQuaternionRotationAxis(orthogonal, XM_PI);
	}

	XMVECTOR axis = XMVector3Cross(v0, v1);
	XMVECTOR axisLenSq = XMVector3LengthSq(axis);

	if (XMVector3Equal(axisLenSq, XMVectorZero()))
		return XMQuaternionIdentity();

	axis = XMVector3Normalize(axis);
	float angle = acosf(dot);
	return XMQuaternionRotationAxis(axis, angle);
}

static void GetPitchYawRollFromMatrix(const XMMATRIX& mat, float* pitch, float* yaw, float* roll)
{
	XMFLOAT4X4 m;
	XMStoreFloat4x4(&m, mat);

	if (pitch) *pitch = asinf(-m._32); // -m[2][1]
	if (yaw)   *yaw = atan2f(m._31, m._33); // m[2][0], m[2][2]
	if (roll)  *roll = atan2f(m._12, m._22); // m[0][1], m[1][1]
}


CScene_1::CScene_1()
{
}

CScene_1::~CScene_1()
{
}

int CScene_1::LoadLineToMesh(const char* filename, std::vector<XMFLOAT3>& vertices, std::vector<std::array<int, 2>>& edges)
{
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
				if ( a > b)
					arr = { b - 1, a - 1 };
				else
					arr = { a - 1, b - 1 };
				edges.push_back(arr);
			}
		}

		return (int)edges.size();
	}
}

void CScene_1::BuildOrderedLinePath(const std::vector<XMFLOAT3>& vertices, const std::vector<std::array<int, 2>>& edges, std::vector<XMFLOAT3>& outPath)
{
	std::unordered_map<int, std::vector<int>> adjacency;

	// 인접 리스트 생성
	for (const auto& e : edges) {
		adjacency[e[0]].push_back(e[1]);
		adjacency[e[1]].push_back(e[0]);
	}

	// 시작점 찾기 (이웃이 1개인 점이 보통 끝점)
	int start = -1;
	for (const auto& a : adjacency) {
		if (a.second.size() == 1) {
			start = a.first;
			break;
		}
	}

	if (start == -1) return; // 닫힌 루프이거나 잘못된 데이터

	std::unordered_set<int> visited;
	int current = start;
	int previous = -1;

	// 경로 따라가기
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

CCamera* CScene_1::CreateCamera()
{
	CCamera* pCamera = new CCamera();
	pCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	pCamera->GeneratePerspectiveProjectionMatrix(1.01f, 500.0f, 60.0f);
	pCamera->SetFOVAngle(60.0f);

	pCamera->GenerateOrthographicProjectionMatrix(1.01f, 50.0f, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	return pCamera;
}

void CScene_1::BuildObjects()
{
	CCubeMesh* t = new CCubeMesh(10.0f, 10.0f, 10.0f);
	m_pCart = new CPlayer();
	m_pCart->SetPosition(0.0f, 0.0, 0.0f);
	m_pCart->SetMesh(t);
	m_pCart->SetColor(RGB(0, 0, 255));
	m_pCart->SetCamera(CreateCamera());
	m_pCart->SetCameraOffset(XMFLOAT3(0.0f, -5.0f, 15.0f));

	m_pDummy = new CPlayer();
	m_pDummy->SetPosition(0.0f, -50.0f, 0.0f);
	m_pDummy->SetMesh(t);
	m_pDummy->SetColor(RGB(0, 0, 255));
	m_pDummy->SetCamera(CreateCamera());
	m_pDummy->SetCameraOffset(XMFLOAT3(0.0f, -5.0f, 15.0f));


	std::vector<XMFLOAT3> tmps;
	LoadLineToMesh("track_line.obj", tmps, m_vTrackEdges);
	BuildOrderedLinePath(tmps, m_vTrackEdges, m_vTrackPoints);

	if (Vector3::Length(m_vTrackPoints.front()) > Vector3::Length(m_vTrackPoints.back()))
	{
		std::reverse(m_vTrackPoints.begin(), m_vTrackPoints.end());
	}


	CTrackMesh* pMesh = new CTrackMesh("track_mesh.obj");
	m_pTrack = new CGameObject();
	m_pTrack->SetPosition(0.0f, -5.0f, 0.0f);
	m_pTrack->SetMesh(pMesh);
	m_pTrack->SetColor(RGB(0, 0, 0));

	m_pPlayer = m_pCart;
}

void CScene_1::ReleaseObjects()
{
}

void CScene_1::ResetObjects()
{
	// 경로 인덱스 초기화
	m_iCurrentPathIndex = 1;

	// 속도 초기화
	m_fCurrentSpeed = 10.0f;

	// 타이머 초기화
	timer = 0.0f;
	m_fMoveSpeed = 20.f;


	// 플레이어 위치 초기화
	if (!m_vTrackPoints.empty())
	{
		XMFLOAT3 startPos = m_vTrackPoints[0];
		m_pPlayer->SetPosition(startPos.x, startPos.y, startPos.z);
	}

}

void CScene_1::Animate(float fElapsedTime)
{
	if (m_pCart == nullptr) return;

	if (m_iCurrentPathIndex >= m_vTrackPoints.size())
	{
		timer += fElapsedTime;
		if (timer >= 3.0f)
		{
			CGameFramework::ChangeScene = true;
			CGameFramework::idx = 3;
		}
		return;
	}

	// 현재 점과 목표 점 위치
	XMFLOAT3 curPos = m_vTrackPoints[m_iCurrentPathIndex - 1];
	XMFLOAT3 targetPos = m_vTrackPoints[m_iCurrentPathIndex];

	// 방향 벡터
	XMFLOAT3 direction = Vector3::Subtract(targetPos, curPos);
	float distance = Vector3::Length(direction);

	

	// 방향 정규화 (목표 방향)
	XMFLOAT3 targetForward = Vector3::Normalize(direction);

	// 현재 방향
	XMFLOAT3 currentForward = m_pCart->GetLook();

	// 부드러운 보간 (Lerp)
	float alpha = 5.0f * fElapsedTime; // 회전 속도 조절
	XMFLOAT3 forward = Vector3::Normalize(Lerp(currentForward, targetForward, alpha));

	// 가속도 적용
	float heightDelta = targetPos.y - curPos.y;
	float accel = 0.0f;
	if (heightDelta < 0) accel = -heightDelta * 20.0f;
	else                 accel = -heightDelta * 0.05f;

	m_fCurrentSpeed += accel * fElapsedTime;
	m_fCurrentSpeed = max(2.0f, min(m_fCurrentSpeed, 70.0f));

	// 좌표계 재구성
	XMFLOAT3 P1 = Vector3::Subtract(curPos, targetPos);
	XMFLOAT3 P2 = Vector3::Add(P1, XMFLOAT3(5.0f, 0.0f, 0.0f));

	// 법선
	XMFLOAT3 normal = ((CTrackMesh*)m_pTrack->m_pMesh)->GetNormal(m_iCurrentPathIndex - 1);

	XMFLOAT3 look = Vector3::Normalize(direction);
	XMFLOAT3 right = Vector3::Normalize(Vector3::CrossProduct(normal, look));
	XMFLOAT3 up = normal;

	m_pCart->SetDirection(right, up, look);


	// 이동
	XMFLOAT3 move = Vector3::ScalarProduct(forward, fElapsedTime * m_fCurrentSpeed);
	m_pCart->Move(move, false);

	// 도착 판정
	XMFLOAT3 prevToCur = Vector3::Subtract(targetPos, curPos);
	XMFLOAT3 curToTarget = Vector3::Subtract(targetPos, m_pCart->GetPosition());

	if (Vector3::DotProduct(prevToCur, curToTarget) < 0)
	{
		m_iCurrentPathIndex++;
		m_pCart->SetPosition(targetPos.x, targetPos.y, targetPos.z);

		if (m_iCurrentPathIndex < m_vTrackPoints.size())
		{
			XMFLOAT3 nextDir = Vector3::Normalize(Vector3::Subtract(m_vTrackPoints[m_iCurrentPathIndex], targetPos));
			m_fTargetYaw = XMConvertToDegrees(atan2f(nextDir.x, nextDir.z)); // 필요 시 유지
		}
		return;
	}

	// 애니메이션
	m_pCart->Animate(fElapsedTime);
	m_pTrack->Animate(fElapsedTime);
	m_pTrack->UpdateBoundingBox();

	// 카메라 처리 (1인칭 시점)
	XMFLOAT3 playerPos = m_pCart->GetPosition();
	look = m_pCart->GetLook();
	XMFLOAT3 cup = m_pCart->GetUp();

	XMFLOAT3 eye = Vector3::Add(playerPos, Vector3::ScalarProduct(cup, 2.0f));
	XMFLOAT3 at = Vector3::Add(eye, look);

	CCamera* cam = m_pCart->GetCamera();
	cam->SetLookAt(eye, at, cup);
}

void CScene_1::Render(HDC hDCFrameBuffer)
{
	if (m_pPlayer == nullptr) return;
	auto pCamera = m_pPlayer->GetCamera();
	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);
	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);



	m_pTrack->Render(hDCFrameBuffer, pCamera);
	if (m_pCart) m_pCart->Render(hDCFrameBuffer, pCamera);
}

void CScene_1::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CScene_1::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case '1':
		{
			if (m_pPlayer == m_pCart) m_pPlayer = m_pDummy;
			else m_pPlayer = m_pCart;
			break;
		}
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			break;
		case 'N':
		case 'n':
			CGameFramework::ChangeScene = true;
			CGameFramework::idx = 3;
			break;
		case VK_ESCAPE:
			CGameFramework::ChangeScene = true;
			CGameFramework::idx = 1;

		default:
			break;
		}
		break;
	default:
		break;
	}
}

CGameObject* CScene_1::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	return nullptr;
}

void CScene_1::ProcessInput(POINT oldCursorPos, HWND hWnd, float m_fElapsedTime)
{
	static UCHAR pKeyBuffer[256];
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
				m_pPlayer->GetCamera()->Rotate(cyMouseDelta, 0.0f, -cxMouseDelta);
			else
				m_pPlayer->GetCamera()->Rotate(cyMouseDelta, cxMouseDelta, 0.0f);
		}
	}

	if (m_pPlayer)
		m_pPlayer->Update(m_fElapsedTime);
}
