#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"
#include "Camera.h"

CGameObject::CGameObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CGameObject::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::Animate(float fTimeElapsed)
{
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (false == bActive) return;
	if (false == IsVisible(pCamera)) return;
	//객체의 정보를 셰이더 변수(상수 버퍼)로 복사한다. 
	UpdateShaderVariables(pd3dCommandList);
	if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
	if (m_pMesh) m_pMesh->Render(pd3dCommandList);
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances)
{
	OnPrepareRender();
	if (m_pMesh) m_pMesh->Render(pd3dCommandList, nInstances);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

//게임 객체의 로컬 z-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32,
		m_xmf4x4World._33)));
}

//게임 객체의 로컬 y-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22,
		m_xmf4x4World._23)));
}

//게임 객체의 로컬 x-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12,
		m_xmf4x4World._13)));
}

//게임 객체를 로컬 x-축 방향으로 이동한다. 
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 로컬 y-축 방향으로 이동한다. 
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 로컬 z-축 방향으로 이동한다. 
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 주어진 각도로 회전한다. 
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	//객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다. 
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CGameObject::ReleaseShaderVariables()
{
}

bool CGameObject::IsVisible(CCamera* pCamera)
{
	OnPrepareRender();
	bool bIsVisible = false;
	BoundingOrientedBox localOBB = m_pMesh->GetBoundingBox();
	BoundingOrientedBox worldOBB;
	localOBB.Transform(worldOBB, XMLoadFloat4x4(&m_xmf4x4World));
	if (pCamera) bIsVisible = pCamera->IsInFrustum(localOBB);
	return(bIsVisible);
}

void CGameObject::GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection)
{
	XMFLOAT4X4 xmf4x4WorldView = Matrix4x4::Multiply(m_xmf4x4World, xmf4x4View);
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(xmf4x4WorldView);
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	//카메라 좌표계의 원점을 모델 좌표계로 변환한다.
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);
	//카메라 좌표계의 점(마우스 좌표를 역변환하여 구한 점)을 모델 좌표계로 변환한다.
	*pxmf3PickRayDirection = Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse);
	//광선의 방향 벡터를 구한다.
	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection, *pxmf3PickRayOrigin));
}

int CGameObject::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance)
{
	int nIntersected = 0;
	if (m_pMesh)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		//모델 좌표계의 광선을 생성한다.
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, &xmf3PickRayDirection);
		//모델 좌표계의 광선과 메쉬의 교차를 검사한다.
		nIntersected = m_pMesh->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, pfHitDistance);
	}
	return(nIntersected);
}

//----------------------------------------------------------------------------------------

CRotatingObject::CRotatingObject()
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}

//----------------------------------------------------------------------------------------

CExplosiveObject::CExplosiveObject()
{

}

CExplosiveObject::~CExplosiveObject()
{
}

void CExplosiveObject::setExplosionMesh(CMesh* mesh)
{
	std::vector<CGameObject*> tmp;
	for (int i = 0; i < EXPLOSION_DEBRISES; ++i)
	{
		XMVECTOR dirVec = ::RandomUnitVectorOnSphere();
		XMFLOAT3 direction;
		XMStoreFloat3(&direction, dirVec);
		auto obj = new CMovingObject(direction, m_fExplosionSpeed, m_fExplosionRotation);
		obj->SetMesh(mesh);

		tmp.emplace_back(obj);
	}

	m_pInstancingShader = new CInstancingShader(tmp);
}

void CExplosiveObject::Animate(float fElapsedTime)
{
	if (!m_bBlowingUp)
	{
		CGameObject::Animate(fElapsedTime);
	}
	else 
	{
		m_fElapsedTimes += fElapsedTime;

		m_pInstancingShader->AnimateObjects(fElapsedTime);

		if (m_fElapsedTimes >= m_fDuration) {
			m_bBlowingUp = false;
			Reset();
		}
	}
}

void CExplosiveObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_bBlowingUp && m_pInstancingShader)
		m_pInstancingShader->Render(pd3dCommandList, pCamera);
	else
		CGameObject::Render(pd3dCommandList, pCamera);
}

void CExplosiveObject::SetExpShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature)
{
	if (m_pInstancingShader)
	{
		m_pInstancingShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
		m_pInstancingShader->BuildObjects(pd3dDevice, pd3dCommandList);
	}
}

void CExplosiveObject::StartExplosion()
{
	m_bBlowingUp = true;
	m_fElapsedTimes = 0.0f;

	// 이 시점에 초기 위치를 지정해줘야 각 debris 위치가 기준점 주변에서 시작됨
	XMFLOAT3 origin = GetPosition();
	m_pInstancingShader->SetPosition(origin);
}

void CExplosiveObject::Reset()
{
	m_bBlowingUp = false;
	m_fElapsedTimes = 0.0f;
	m_pInstancingShader->Reset();
}

CMovingObject::CMovingObject(XMFLOAT3 dir, float speed, float rotationSpeed)
	: m_xmf3Direction(dir), m_fSpeed(speed), m_fRotationSpeed(rotationSpeed)
{
	m_fTimeElapsed = 0.0f;

	// 기본적으로 방향 단위 벡터로 정규화 (안전성 확보)
	float length = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
	if (length > 0.0f)
	{
		m_xmf3Direction.x /= length;
		m_xmf3Direction.y /= length;
		m_xmf3Direction.z /= length;
	}

	// 월드 행렬 초기화 (Identity 또는 필요 시 위치/회전 초기화)
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

void CMovingObject::Animate(float fElapsedTime)
{
	m_fTimeElapsed += fElapsedTime;

	if (m_fTimeElapsed <= 2.0f)
	{
		XMFLOAT3 position = GetPosition();

		// 위치 이동
		position.x += m_xmf3Direction.x * m_fSpeed * fElapsedTime;
		position.y += m_xmf3Direction.y * m_fSpeed * fElapsedTime;
		position.z += m_xmf3Direction.z * m_fSpeed * fElapsedTime;
		SetPosition(position);

		// 회전
		XMMATRIX mtxWorld = XMLoadFloat4x4(&m_xmf4x4World);
		XMMATRIX mtxRot = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Direction), XMConvertToRadians(m_fRotationSpeed * fElapsedTime));
		mtxWorld = XMMatrixMultiply(mtxRot, mtxWorld);
		XMStoreFloat4x4(&m_xmf4x4World, mtxWorld);
	}
}

void CMovingObject::Reset()
{
	m_fTimeElapsed = 0.0f;
}

//------------------------------------------------

CBulletObject::CBulletObject(float fEffectiveRange)
{
	m_fBulletEffectiveRange = fEffectiveRange;
}

CBulletObject::~CBulletObject()
{
}

void CBulletObject::SetFirePosition(XMFLOAT3 xmf3FirePosition)
{
	m_xmf3FirePosition = xmf3FirePosition;
	SetPosition(xmf3FirePosition);
}

void CBulletObject::Reset()
{
	m_pLockedObject = NULL;
	m_fElapsedTimeAfterFire = 0;
	m_fMovingDistance = 0;
	m_fRotationAngle = 0.0f;
	bActive = false;
}

void CBulletObject::Animate(float fElapsedTime)
{
	m_fElapsedTimeAfterFire += fElapsedTime;

	// 회전 속도를 이동 거리로 사용하고 있음 (이름 혼란 주의)
	float fDistance = m_fRotationSpeed * fElapsedTime;

	// 유도: LockingDelayTime 이후에만 유도 활성화
	if (m_pLockedObject)
	{
		XMFLOAT3 xmf3Position = GetPosition();
		XMVECTOR xmvPosition = XMLoadFloat3(&xmf3Position);

		XMFLOAT3 xmf3TargetPosition = m_pLockedObject->GetPosition();
		XMVECTOR xmvTargetPosition = XMLoadFloat3(&xmf3TargetPosition);

		XMVECTOR xmvToTarget = XMVector3Normalize(xmvTargetPosition - xmvPosition);
		XMVECTOR xmvCurrent = XMLoadFloat3(&m_xmf3MovingDirection);
		XMVECTOR xmvSmoothed = XMVector3Normalize(XMVectorLerp(xmvCurrent, xmvToTarget, 0.25f));
		XMStoreFloat3(&m_xmf3MovingDirection, xmvSmoothed);
	}

	// 이동
	XMFLOAT3 xmf3Move = Vector3::ScalarProduct(m_xmf3MovingDirection, fDistance, false);
	XMFLOAT3 xmf3Position = Vector3::Add(GetPosition(), xmf3Move);
	SetPosition(xmf3Position);
	m_fMovingDistance += fDistance;


	// 유효 범위 또는 생존 시간 초과 시 리셋
	if ((m_fMovingDistance > m_fBulletEffectiveRange) || (m_fElapsedTimeAfterFire > m_fLockingTime))
		Reset();
}

CTankObject::CTankObject()
{
	m_pBody = new CGameObject();
	
}

CTankObject::~CTankObject()
{
}

void CTankObject::Animate(float fElapsedTime)
{
	CExplosiveObject::Animate(fElapsedTime);
	m_pBody->SetPosition(GetPosition().x, GetPosition().y-1.0f, GetPosition().z);
}

void CTankObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_bBlowingUp && m_pInstancingShader)
		m_pInstancingShader->Render(pd3dCommandList, pCamera);
	else
	{
		if (m_pBody)
		{

			m_pBody->Render(pd3dCommandList, pCamera);
		}
		CGameObject::Render(pd3dCommandList, pCamera);
	}

	
}

int CTankObject::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance)
{
	int nIntersected = 0;
	if (m_pBody)
	{
		nIntersected += m_pBody->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, pfHitDistance);
	}
	if (m_pMesh)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		//모델 좌표계의 광선을 생성한다.
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, &xmf3PickRayDirection);
		//모델 좌표계의 광선과 메쉬의 교차를 검사한다.
		nIntersected += m_pMesh->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, pfHitDistance);
	}
	return(nIntersected);
}
