#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	XMFLOAT4X4 m_xmf4x4World;
protected:
	CMesh* m_pMesh = NULL;
	CShader* m_pShader = NULL;
public:
	void ReleaseUploadBuffers();
	virtual void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances);

public:
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	//���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�. 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	//���� ��ü�� ��ġ�� �����Ѵ�. 
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	//���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�. 
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

public:
	//��� ���۸� �����Ѵ�. 
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	//��� ������ ������ �����Ѵ�.
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

public:
	//���� ��ü�� ī�޶� ���δ� ���� �˻��Ѵ�.
	bool IsVisible(CCamera* pCamera = NULL);

	bool bActive = true; //���� ��ü�� Ȱ��ȭ �������� ���θ� ��Ÿ����.

public:
	//�� ��ǥ���� ��ŷ ������ �����Ѵ�.
	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection);
	//ī�޶� ��ǥ���� �� ���� ���� �� ��ǥ���� ��ŷ ������ �����ϰ� ��ü���� ������ �˻��Ѵ�.
	int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);

	BoundingOrientedBox GetBoundingBox()
	{
		BoundingOrientedBox boundingBox = m_pMesh->GetBoundingBox();
		boundingBox.Transform(boundingBox, XMLoadFloat4x4(&m_xmf4x4World));
		return boundingBox;
	}
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();
private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) {
		m_xmf3RotationAxis =
			xmf3RotationAxis;
	}
	virtual void Animate(float fTimeElapsed);
};

//----------------------------------------------------

class CExplosiveObject : public CGameObject
{
public:
	CExplosiveObject();
	virtual ~CExplosiveObject();

	bool m_bBlowingUp = false;
	float m_fElapsedTimes = 0.0f;
	float m_fDuration = 2.0f;
	float m_fExplosionSpeed = 10.0f;
	float m_fExplosionRotation = 720.0f;

	CInstancingShader* m_pInstancingShader = nullptr;
	CMesh* m_expMesh = nullptr;

	void setExplosionMesh(CMesh* mesh);
	void Animate(float fElapsedTime);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void SetExpShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature);
	void StartExplosion();
	void Reset();
};

//----------------------------------------------------
// explosive���� ����� �׳� �����̴� ����
class CMovingObject : public CGameObject
{
public:
	CMovingObject(XMFLOAT3 dir, float speed, float rotationSpeed);
	virtual void Animate(float fElapsedTime) override;
	void Reset();
private:
	XMFLOAT3 m_xmf3Direction;
	float m_fSpeed;
	float m_fRotationSpeed;
	float m_fTimeElapsed = 0.0f;
};

//----------------------------------------------------

class CBulletObject : public CGameObject
{
public:
	CBulletObject(float fEffectiveRange);
	virtual ~CBulletObject();

public:
	virtual void Animate(float fElapsedTime);

	float						m_fBulletEffectiveRange = 50.0f;
	float						m_fMovingDistance = 0.0f;
	float						m_fRotationAngle = 0.0f;
	XMFLOAT3					m_xmf3FirePosition = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3					m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);

	float						m_fElapsedTimeAfterFire = 0.0f;
	float						m_fLockingTime = 4.0f;
	float						m_fRotationSpeed = 20.0f;
	CGameObject* m_pLockedObject = NULL;

	void SetFirePosition(XMFLOAT3 xmf3FirePosition);
	void SetMovingDirection(XMFLOAT3 xmf3Direction) {
		m_xmf3MovingDirection = xmf3Direction;
	}
	void Reset();
};

//----------------------------------------------------

class CTankObject : public CExplosiveObject
{
public:
	CGameObject* m_pBody = nullptr; // ��ũ ��ü ��ü

	XMFLOAT3 m_xmf3MoveDirection = XMFLOAT3(0.0f, 0.0f, 1.0f); // �⺻ ���� ����
	float m_fSpeed = 1.0f;

	CTankObject();
	virtual ~CTankObject();
	void SetBody(CMesh* pBody) { m_pBody->SetMesh(pBody); }
	CGameObject* GetBody() const { return m_pBody; }
	void Animate(float fElapsedTime) override;
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
	void SetShader(CShader* pShader)
	{
		m_pBody->SetShader(pShader);
		CGameObject::SetShader(pShader);
	}
	int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);
};

//-------------------------------------------------

class CObstacles : public CGameObject
{
public:
	CObstacles(XMFLOAT3 dir, float speed);
	virtual void Animate(float fElapsedTime) override;
	void Reset();
private:
	XMFLOAT3 m_xmf3Direction;
	float m_fSpeed;
};