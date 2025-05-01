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
		return XMQuaternionIdentity(); // ���� ����
	if (dot <= -1.0f + 1e-6f)
	{
		// �ݴ� ������ ��� �� ������ ���� ���ͷ� �� ����
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

	// ���� ����Ʈ ����
	for (const auto& e : edges) {
		adjacency[e[0]].push_back(e[1]);
		adjacency[e[1]].push_back(e[0]);
	}

	// ������ ã�� (�̿��� 1���� ���� ���� ����)
	int start = -1;
	for (const auto& a : adjacency) {
		if (a.second.size() == 1) {
			start = a.first;
			break;
		}
	}

	if (start == -1) return; // ���� �����̰ų� �߸��� ������

	std::unordered_set<int> visited;
	int current = start;
	int previous = -1;

	// ��� ���󰡱�
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
	// ��� �ε��� �ʱ�ȭ
	m_iCurrentPathIndex = 1;

	// �ӵ� �ʱ�ȭ
	m_fCurrentSpeed = 10.0f;

	// Ÿ�̸� �ʱ�ȭ
	timer = 0.0f;
	m_fMoveSpeed = 20.f;


	// �÷��̾� ��ġ �ʱ�ȭ
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

	// ���� ���� ��ǥ �� ��ġ
	XMFLOAT3 curPos = m_vTrackPoints[m_iCurrentPathIndex - 1];
	XMFLOAT3 targetPos = m_vTrackPoints[m_iCurrentPathIndex];

	// ���� ����
	XMFLOAT3 direction = Vector3::Subtract(targetPos, curPos);
	float distance = Vector3::Length(direction);

	

	// ���� ����ȭ (��ǥ ����)
	XMFLOAT3 targetForward = Vector3::Normalize(direction);

	// ���� ����
	XMFLOAT3 currentForward = m_pCart->GetLook();

	// �ε巯�� ���� (Lerp)
	float alpha = 5.0f * fElapsedTime; // ȸ�� �ӵ� ����
	XMFLOAT3 forward = Vector3::Normalize(Lerp(currentForward, targetForward, alpha));

	// ���ӵ� ����
	float heightDelta = targetPos.y - curPos.y;
	float accel = 0.0f;
	if (heightDelta < 0) accel = -heightDelta * 20.0f;
	else                 accel = -heightDelta * 0.05f;

	m_fCurrentSpeed += accel * fElapsedTime;
	m_fCurrentSpeed = max(2.0f, min(m_fCurrentSpeed, 70.0f));

	// ��ǥ�� �籸��
	XMFLOAT3 P1 = Vector3::Subtract(curPos, targetPos);
	XMFLOAT3 P2 = Vector3::Add(P1, XMFLOAT3(5.0f, 0.0f, 0.0f));

	// ����
	XMFLOAT3 normal = ((CTrackMesh*)m_pTrack->m_pMesh)->GetNormal(m_iCurrentPathIndex - 1);

	XMFLOAT3 look = Vector3::Normalize(direction);
	XMFLOAT3 right = Vector3::Normalize(Vector3::CrossProduct(normal, look));
	XMFLOAT3 up = normal;

	m_pCart->SetDirection(right, up, look);


	// �̵�
	XMFLOAT3 move = Vector3::ScalarProduct(forward, fElapsedTime * m_fCurrentSpeed);
	m_pCart->Move(move, false);

	// ���� ����
	XMFLOAT3 prevToCur = Vector3::Subtract(targetPos, curPos);
	XMFLOAT3 curToTarget = Vector3::Subtract(targetPos, m_pCart->GetPosition());

	if (Vector3::DotProduct(prevToCur, curToTarget) < 0)
	{
		m_iCurrentPathIndex++;
		m_pCart->SetPosition(targetPos.x, targetPos.y, targetPos.z);

		if (m_iCurrentPathIndex < m_vTrackPoints.size())
		{
			XMFLOAT3 nextDir = Vector3::Normalize(Vector3::Subtract(m_vTrackPoints[m_iCurrentPathIndex], targetPos));
			m_fTargetYaw = XMConvertToDegrees(atan2f(nextDir.x, nextDir.z)); // �ʿ� �� ����
		}
		return;
	}

	// �ִϸ��̼�
	m_pCart->Animate(fElapsedTime);
	m_pTrack->Animate(fElapsedTime);
	m_pTrack->UpdateBoundingBox();

	// ī�޶� ó�� (1��Ī ����)
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
