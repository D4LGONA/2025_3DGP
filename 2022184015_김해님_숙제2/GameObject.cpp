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
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��. 
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
	if (false == IsVisible(pCamera)) return;
	//��ü�� ������ ���̴� ����(��� ����)�� �����Ѵ�. 
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

//���� ��ü�� ���� z-�� ���͸� ��ȯ�Ѵ�. 
XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32,
		m_xmf4x4World._33)));
}

//���� ��ü�� ���� y-�� ���͸� ��ȯ�Ѵ�. 
XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22,
		m_xmf4x4World._23)));
}

//���� ��ü�� ���� x-�� ���͸� ��ȯ�Ѵ�. 
XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12,
		m_xmf4x4World._13)));
}

//���� ��ü�� ���� x-�� �������� �̵��Ѵ�. 
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//���� ��ü�� ���� y-�� �������� �̵��Ѵ�. 
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//���� ��ü�� ���� z-�� �������� �̵��Ѵ�. 
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//���� ��ü�� �־��� ������ ȸ���Ѵ�. 
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
	//��ü�� ���� ��ȯ ����� ��Ʈ ���(32-��Ʈ ��)�� ���Ͽ� ���̴� ����(��� ����)�� �����Ѵ�. 
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CGameObject::ReleaseShaderVariables()
{
}

bool CGameObject::IsVisible(CCamera* pCamera)
{
	OnPrepareRender();
	bool bIsVisible = false;
	BoundingOrientedBox xmBoundingBox = m_pMesh->GetBoundingBox();
	//�� ��ǥ���� �ٿ�� �ڽ��� ���� ��ǥ��� ��ȯ�Ѵ�.
	xmBoundingBox.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
	if (pCamera) bIsVisible = pCamera->IsInFrustum(xmBoundingBox);
	return(bIsVisible);
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

	// �� ������ �ʱ� ��ġ�� ��������� �� debris ��ġ�� ������ �ֺ����� ���۵�
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

	// �⺻������ ���� ���� ���ͷ� ����ȭ (������ Ȯ��)
	float length = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
	if (length > 0.0f)
	{
		m_xmf3Direction.x /= length;
		m_xmf3Direction.y /= length;
		m_xmf3Direction.z /= length;
	}

	// ���� ��� �ʱ�ȭ (Identity �Ǵ� �ʿ� �� ��ġ/ȸ�� �ʱ�ȭ)
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

void CMovingObject::Animate(float fElapsedTime)
{
	m_fTimeElapsed += fElapsedTime;

	if (m_fTimeElapsed <= 2.0f)
	{
		XMFLOAT3 position = GetPosition();

		// ��ġ �̵�
		position.x += m_xmf3Direction.x * m_fSpeed * fElapsedTime;
		position.y += m_xmf3Direction.y * m_fSpeed * fElapsedTime;
		position.z += m_xmf3Direction.z * m_fSpeed * fElapsedTime;
		SetPosition(position);

		// ȸ��
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
