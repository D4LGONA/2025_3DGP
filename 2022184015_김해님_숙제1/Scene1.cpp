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

CScene_1::CScene_1()
{
}

CScene_1::~CScene_1()
{
}

int CScene_1::LoadLineToMesh(const char* filename, std::vector<XMFLOAT3>& vertices, std::vector<std::array<int, 2>>& edges)
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

void CScene_1::BuildOrderedLinePath(const std::vector<XMFLOAT3>& vertices, const std::vector<std::array<int, 2>>& edges, std::vector<XMFLOAT3>& outPath)
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
	m_pDummy->SetPosition(0.0f, 100.0f, 0.0f);
	m_pDummy->SetMesh(t);
	m_pDummy->SetColor(RGB(0, 0, 255));
	m_pDummy->SetCamera(CreateCamera());
	m_pDummy->SetCameraOffset(XMFLOAT3(0.0f, 50.0f, 15.0f));


	std::vector<XMFLOAT3> tmps;
	LoadLineToMesh("track_line.obj", tmps, m_vTrackEdges);
	BuildOrderedLinePath(tmps, m_vTrackEdges, m_vTrackPoints);

	if (Vector3::Length(m_vTrackPoints.front()) > Vector3::Length(m_vTrackPoints.back()))
	{
		std::reverse(m_vTrackPoints.begin(), m_vTrackPoints.end());
	}

	CTrackMesh* pMesh = new CTrackMesh("track_mesh_fixed.obj");
	m_pTrack = new CGameObject();
	m_pTrack->SetPosition(0.0f, -5.0f, 0.0f);
	m_pTrack->SetMesh(pMesh);
	m_pTrack->SetColor(RGB(0, 0, 0));

	m_pPlayer = m_pCart;

	XMFLOAT3 playerPos = m_pCart->GetPosition();
	XMFLOAT3 look = Vector3::ScalarProduct(m_pCart->GetLook(), 1.0f);
	XMFLOAT3 cup = Vector3::ScalarProduct(m_pCart->GetUp(), -1.0f);

	XMFLOAT3 eye = Vector3::Add(playerPos, Vector3::ScalarProduct(cup, 5.0f));
	XMFLOAT3 at = Vector3::Add(eye, look);

	CCamera* cam = m_pCart->GetCamera();
	cam->SetLookAt(eye, at, cup);
}

void CScene_1::ReleaseObjects()
{
	if (m_pCart) delete m_pCart;
	if (m_pDummy) delete m_pDummy;
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

void CScene_1::ResetObjects()
{
	m_iCurrentPathIndex = 1;
	m_fCurrentSpeed = 10.0f;
	timer = 0.0f;
	m_fMoveSpeed = 20.f;

	if (!m_vTrackPoints.empty())
	{
		XMFLOAT3 startPos = m_vTrackPoints[0];
		m_pPlayer->SetPosition(startPos.x, startPos.y, startPos.z);
	}

}

void CScene_1::Animate(float fElapsedTime)
{
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

	XMFLOAT3 curPos = m_vTrackPoints[m_iCurrentPathIndex - 1];
	XMFLOAT3 targetPos = m_vTrackPoints[m_iCurrentPathIndex];

	XMFLOAT3 direction = Vector3::Subtract(targetPos, curPos);
	float distance = Vector3::Length(direction);

	XMFLOAT3 targetForward = Vector3::Normalize(direction);
	XMFLOAT3 currentForward = m_pCart->GetLook();

	float alpha = 5.0f * fElapsedTime;
	XMFLOAT3 forward = Vector3::Normalize(Lerp(currentForward, targetForward, alpha));

	float heightDelta = targetPos.y - curPos.y;
	float accel = 0.0f;
	if (heightDelta < 0) accel = -heightDelta * 30.f;
	else                 accel = -heightDelta * 0.2f;

	m_fCurrentSpeed += accel * fElapsedTime;
	m_fCurrentSpeed = max(2.0f, min(m_fCurrentSpeed, 70.0f));

	XMFLOAT3 normal = ((CTrackMesh*)m_pTrack->m_pMesh)->GetNormal(m_iCurrentPathIndex - 1);

	if (Vector3::DotProduct(normal, m_pCart->GetUp()) < 0)
		normal = Vector3::ScalarProduct(normal, -1.0f);

	XMFLOAT3 look = Vector3::Normalize(direction);
	XMFLOAT3 right = Vector3::Normalize(Vector3::CrossProduct(look, normal));
	XMFLOAT3 up = normal;

	XMFLOAT3 prevLook = m_pCart->GetLook();
	XMFLOAT3 prevUp = m_pCart->GetUp();
	XMFLOAT3 prevRight = m_pCart->GetRight();

	XMFLOAT3 smoothLook = Vector3::Normalize(Lerp(prevLook, look, alpha));
	XMFLOAT3 smoothUp = Vector3::Normalize(Lerp(prevUp, up, alpha));
	XMFLOAT3 smoothRight = Vector3::Normalize(Vector3::CrossProduct(smoothUp, smoothLook));
	smoothUp = Vector3::CrossProduct(smoothLook, smoothRight);

	m_pCart->SetDirection(smoothRight, smoothUp, smoothLook);

	XMFLOAT3 move = Vector3::ScalarProduct(forward, fElapsedTime * m_fCurrentSpeed);
	XMFLOAT3 curPosCart = m_pCart->GetPosition();
	XMFLOAT3 nextPos = Vector3::Add(curPosCart, move);

	XMFLOAT3 curToTarget = Vector3::Subtract(targetPos, curPos);
	XMFLOAT3 nextToTarget = Vector3::Subtract(targetPos, nextPos);

	if (Vector3::DotProduct(curToTarget, nextToTarget) < 0)
	{
		m_iCurrentPathIndex++;
		m_pCart->SetPosition(targetPos.x, targetPos.y, targetPos.z);
	}
	else
	{
		m_pCart->Move(move, false);
	}

	if (b_LockingCamera) 
	{
		// 카메라 처리
		XMFLOAT3 playerPos = m_pCart->GetPosition();
		XMFLOAT3 look = Vector3::ScalarProduct(m_pCart->GetLook(), 1.0f);
		XMFLOAT3 cup = Vector3::ScalarProduct(m_pCart->GetUp(), -1.0f);

		XMFLOAT3 eye = Vector3::Add(playerPos, Vector3::ScalarProduct(cup, 5.0f));
		XMFLOAT3 at = Vector3::Add(eye, look);

		CCamera* cam = m_pCart->GetCamera();
		cam->SetLookAt(eye, at, cup);
	}

	// 애니메이션
	m_pCart->Animate(fElapsedTime);
	m_pTrack->Animate(fElapsedTime);
	m_pTrack->UpdateBoundingBox();
}


void CScene_1::Render(HDC hDCFrameBuffer)
{
	if (m_pPlayer == nullptr) return;
	auto pCamera = m_pPlayer->GetCamera();
	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);
	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);

	m_pTrack->Render(hDCFrameBuffer, pCamera);
	if (m_pCart && b_LockingCamera == false) m_pCart->Render(hDCFrameBuffer, pCamera);
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
		case 'F':
		case 'f':
			b_LockingCamera = !b_LockingCamera;
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
